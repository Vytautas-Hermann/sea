#include "sealib/reversedfs.h"
#include <cmath>
#include <iostream>

using namespace Sealib;  // NOLINT

void ReverseDFS::storeTime(unsigned df, uint u) {
  if (df == 0) {
    if (d.get(u) == r + 1) {
      d.insert(u, j);
    }
  } else if (df == 1) {
    if (f.get(u) == r + 1) {
      f.insert(u, j);
    }
  }
}

void ReverseDFS::updateInterval(uint actions, bool end) {
  IntervalData &ci = i[j];
  Pair top = s.top();
  if (ci.size == 0 && ci.h1.head() == INVALID) {
    ci.h1 = top;
  }
  ci.size += actions;
  if (ns < ci.hdc && s.top().head() != INVALID) {
    ci.hd = top;
    ci.hdc = ns;
  }
  if (ci.size > w || end) {
    ci.h2 = top;
    ci.c1 = firstCall;
    firstCall = UserCall();
    std::cout << "H" << j << " = (" << ci.h1.head() << "," << ci.h1.tail()
              << ")\n";
    std::cout << "Ĥ" << j << " = (" << ci.hd.head() << "," << ci.hd.tail()
              << ")\n";
    std::cout << "H'" << j << " = (" << ci.h2.head() << "," << ci.h2.tail()
              << ")\n";
    std::cout << "c" << j << " = " << ci.c1.type << ":" << ci.c1.u << ","
              << ci.c1.k << "\n";
    j++;
    if (j >= r) throw std::out_of_range("too many intervals");
    updateInterval(0);
  }
}
void ReverseDFS::setCall(UserCall call) {
  if (firstCall == UserCall()) {
    firstCall = call;
  }
}

void ReverseDFS::process_recording(uint u0) {
  updateInterval(0);
  s.push(Pair(u0, 0));
  ns = 1;
  Pair x;
  while (!s.isEmpty()) {
    uint actions = 0;
    int sr = s.pop(&x);
    if (sr == DFS_DO_RESTORE) {
      restore_top(u0, g, &c, &s);
      s.pop(&x);
    } else if (sr == DFS_NO_MORE_NODES) {
      return;
    }
    uint u = x.head(), k = x.tail();
    if (c.get(u) == DFS_WHITE) {
      // preprocess(u);
      setCall({UserCall::preprocess, u});
      storeTime(0, u);
      c.insert(u, DFS_GRAY);
      actions++;
    }
    if (k < g->getNodeDegree(u)) {
      s.push(Pair(u, k + 1));
      uint v = g->head(u, k);
      // preexplore(u, v);
      setCall({UserCall::preexplore, u, k});
      if (c.get(v) == DFS_WHITE) {
        s.push(Pair(v, 0));
        actions++;
        ns++;
      }
      // (postexplore can never be a first call in an interval)
    } else {
      c.insert(u, DFS_BLACK);
      storeTime(1, u);
      // postprocess(u);
      setCall({UserCall::postprocess, u});
      actions++;
      ns--;
    }
    updateInterval(actions);
  }
}

void ReverseDFS::init() {
  for (uint u = 0; u < n; u++) {
    if (c.get(u) == DFS_WHITE) process_recording(u);
  }
  updateInterval(true);
}

std::stack<Pair> *ReverseDFS::reconstructPart(Pair from, Pair to) {
  std::cout << "reconstruct " << j << " " << from.head() << " " << to.head()
            << "\n";
  for (uint a = 0; a < n; a++) {
    if (f.get(a) == j) std::cout << a << " ";
  }
  std::cout << "\n";
  std::stack<Pair> *sj = new std::stack<Pair>();
  std::set<uint> tmp;
  if (to.head() != INVALID) {
    if (from.head() == INVALID) {
      sj->push(to);  // is this enough?
    } else {
      Pair a = Pair(from.head(), /*from.tail() - 1*/ 0);
      tmp.insert(a.head());
      do {
        uint u = a.head();
        std::cout << "u=" << u << "  ";
        if (u == to.head()) {
          sj->push(to);
        } else {
          bool found = false;
          for (uint k = a.tail(); k < g->getNodeDegree(u); k++) {
            uint v = g->head(u, k);
            std::cout << " -> " << v << "(c" << c.get(v) << ",d" << d.get(v)
                      << ",f" << f.get(v) << ")\n";
            if (f.get(v) == j && tmp.find(v) == tmp.end()) {
              // we need a fast way for recoloring;
              // until then: using a temp. set
              a = Pair(v, 0);
              sj->push(Pair(u, k + 1));
              tmp.insert(v);
              found = true;
              break;
            }
          }
          if (!found) {
            throw std::logic_error("no white path found in reconstruction");
          }
        }
      } while (sj->top() != to);
    }
  } else {
    std::cout << "skipping reconstruction: H is invalid (bottom of stack)\n";
  }
  return sj;
}

std::vector<UserCall> *ReverseDFS::simulate(std::stack<Pair> *sj, Pair until,
                                            UserCall first) {
  std::cout << "simulate " << j << " " << until.head() << "\n";
  std::vector<UserCall> *ret = new std::vector<UserCall>();

  if (first.type == UserCall::preprocess && sj->empty()) {
    sj->push(Pair(first.u, 0));
  }
  if (sj->empty()) {
    delete ret;
    throw std::logic_error("stack empty");
  }

  while (sj->top() != until) {
    Pair x = sj->top();
    sj->pop();
    uint u = x.head(), k = x.tail();
    // std::cout << "top: " << u << "," << k << "\n";
    if (c.get(u) == DFS_WHITE) {
      // std::cout << "preproc " << u << "\n";
      ret->emplace_back(UserCall(UserCall::Type::preprocess, u));
      c.insert(u, DFS_GRAY);
    }
    if (k < g->getNodeDegree(u)) {
      sj->push(Pair(u, k + 1));
      uint v = g->head(u, k);
      if (c.get(v) == DFS_WHITE) {
        // std::cout << "preexp " << u << "," << v << "\n";
        ret->emplace_back(UserCall(UserCall::Type::preexplore, u, k));
        sj->push(Pair(v, 0));
      } else {
        // no postexplore: will be simulated later
        // std::cout << " == pre+postexplore " << u << "," << v << " == \n";
      }
    } else {
      c.insert(u, DFS_BLACK);
      // std::cout << "postproc " << u << "\n";
      ret->emplace_back(UserCall(UserCall::Type::postprocess, u));
      if (sj->size() > 0) {
        uint pu = sj->top().head(), pk = sj->top().tail();
        // std::cout << "postexp " << pu << "," << u << "\n";
        ret->emplace_back(UserCall(UserCall::Type::postexplore, pu,
                                   pk - 1));  // this postexplore is necessary
      }
    }
    if (sj->empty()) {
      bool a = false;
      for (uint b = 0; b < n; b++) {
        if (c.get(b) == DFS_WHITE && d.get(b) == j) {
          sj->push(Pair(b, 0));
          a = true;
          break;
        }
      }
      if (!a) break;
    }
  }
  return ret;
}

bool ReverseDFS::more() { return !(j == 0 && majorI == major->rend()); }

UserCall ReverseDFS::next() {
  if (majorI != major->rend()) {
    if (!waitStep) {
      UserCall &cc = *majorI;
      if (previous.u == cc.u && (cc.type == UserCall::preprocess ||
                                 cc.type == UserCall::postexplore)) {
        int k = static_cast<int>(previous.k - 1);
        if (k >= 0) {
          /*std::cout << "this: " << (cc.type == UserCall::preprocess
                                        ? "preproc"
                                        : cc.type == UserCall::postexplore
                                              ? "postexp"
                                              : "?")
                    << " " << cc.u << "," << cc.k << ";  previous: "
                    << (previous.type == UserCall::preexplore ? "preexp"
                                                              : "postproc")
                    << " " << previous.u << "," << previous.k << ";  k=" << k
                    << "\n";*/

          while (k >= static_cast<int>(cc.k)) {
            uint v = g->head(cc.u, static_cast<uint>(k));
            minor.emplace_back(UserCall(UserCall::preexplore, cc.u, v));
            minor.emplace_back(UserCall(UserCall::postexplore, cc.u, v));
            // std::cout << " >>> minor: " << cc.u << "," << v << "\n";
            k--;
          }
          if (minor.size() > 0) {
            waitStep = true;
          }
        }
      }
    }
    if (minor.size() == 0) {
      previous = *majorI;
      waitStep = false;
      return *(majorI++);
    } else {
      UserCall m = minor.front();
      minor.pop_front();
      return m;
    }
  } else {  // build new sequence
    j--;
    for (uint a = 0; a < n; a++) {
      if (f.get(a) < j) {
        c.insert(a, DFS_BLACK);
      } else if (d.get(a) < j) {
        c.insert(a, DFS_GRAY);
      } else {
        c.insert(a, DFS_WHITE);
      }
    }
    std::stack<Pair> *sj = reconstructPart(i[j].hd, i[j].h1);
    delete major;
    major = simulate(sj, i[j].h2, i[j].c1);
    std::cout << "end of simulation " << j << "\n";
    majorI = major->rbegin();
    return next();
  }
}

ReverseDFS::ReverseDFS(Graph *graph)
    : g(graph),
      n(g->getOrder()),
      r(static_cast<uint>(ceil(8 * log2(n)))),
      w(static_cast<uint>(ceil(n / log2(n)))),
      c(n, 3),
      d(n, r + 1),
      f(n, r + 1),
      s(n, g, &c),
      i(new IntervalData[r]),
      major{new std::vector<UserCall>(0)},
      majorI(major->rend()) {
  std::cout << "r=" << r << ", w=" << w << "\n";
  for (uint a = 0; a < n; a++) {
    d.insert(a, r + 1);
    f.insert(a, r + 1);
  }
}

ReverseDFS::~ReverseDFS() {
  delete[] i;
  delete major;
}

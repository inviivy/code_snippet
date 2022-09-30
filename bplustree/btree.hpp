#pragma once
#include <cassert>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <queue>

namespace TinySql {
template <class T, uint32_t BTREE_ORDER = 3> class btree {
private:
  enum STATE { BT_OVERFLOW, BT_UNDERFLOW, NORMAL };
  struct node {
    uint32_t count;                  /* 当前node中key的个数 */
    T data[BTREE_ORDER + 1];         /* 多存储一个T, 用于split */
    node *children[BTREE_ORDER + 2]; /* 多存储一个children指针, children[0] ==
                                        nullptr表示是孩子 */
    node *right = nullptr;           /* 兄弟节点 */

    node() : count(0) {
      /* 初始时所有的children为nullptr */
      for (uint32_t i = 0; i < BTREE_ORDER + 2; ++i) {
        children[i] = nullptr;
      }
    }

    /**
     * @brief 在下标pos处插入key val
     *
     * @param pos
     * @param val
     */
    void insert_in_node(uint32_t pos, const T &val) {
      uint32_t j = count;
      while (j > pos) {
        data[j] = data[j - 1];
        children[j + 1] = children[j]; /* children个数 = key个数 + 1 */
        --j;
      }
      /* 此时的 j == pos */
      assert(j == pos);
      data[j] = val;
      children[j + 1] = children[j];
      /**
       * @brief 这里的插入非常奇怪
       * 比如在[1, 5]中插入3
       * children的[3, 5)是原来[1, 5)的children
       * 那么children这里[1,  3)的children是什么呢?
       */
      ++count;
    }

    /**
     * @brief 删除pos下标的key,
     * 也就是我们需要分清传入的pos是ptr基于children的还是基于key的
     * 注意children的动作, 这里的指针仍然不一致
     * 需要后续修改
     * @param pos
     */
    void delete_in_node(int pos) {
      for (uint32_t i = pos; i < count; ++i) {
        data[i] = data[i + 1];
        /* 大于等于data[pos]的哪个指针会被覆盖 */
        children[i + 1] = children[i + 2];
      }
      --count;
    }

    bool is_overflow() { return count > BTREE_ORDER; }
    bool is_underflow() { return count < std::floor(BTREE_ORDER / 2.0); }
  };

public:
  btree() { root = new node; }
  ~btree() {
    if (root != nullptr) {
      std::cout << "root != nullptr\n";
      std::queue<node *> que;
      que.push(root);
      root = nullptr;
      while (!que.empty()) {
        node *tmp = que.front();
        que.pop();
        /* 叶子节点没有children */
        if (tmp->children[0] != nullptr) {
          for (uint32_t i = 0; i < tmp->count + 1; ++i) {
            que.push(tmp->children[i]);
          }
        }
        delete tmp;
      }
    }
  }

  void insert(const T &val) {
    auto state = insert(root, val);
    if (state == STATE::BT_OVERFLOW) {
      split_root(root, val);
    }
  }

  STATE insert(node *ptr, const T &val) {
    uint32_t pos = 0;
    while (pos < ptr->count && ptr->data[pos] < val) {
      ++pos; /* 找到第一个<val的 pos */
    }
    if (ptr->children[pos] != nullptr) {
      /* 说明是内部节点, 不是leaf */
      STATE state = insert(ptr->children[pos], val); /* 递归insert */
      if (state == STATE::BT_OVERFLOW) {
        /* 处理子节点溢出, 分裂叶子节点, 然后将对应的key插入到当前ptr */
        split(ptr, pos);
      }
    } else {
      /* 是叶子节点: todo: 处理重复key */
      ptr->insert_in_node(pos, val);
    }

    /* 判断当前节点是否符合btree定义 */
    return ptr->is_overflow() == true ? STATE::BT_OVERFLOW : STATE::NORMAL;
  }

  /**
   * @brief 分裂ptr->children[pos]节点
   *
   * @param ptr
   * @param pos chidren下标
   */
  void split(node *ptr, int pos) {
    node *node_in_overflow = ptr->children[pos];
    node *child1 = node_in_overflow;
    node *child2 = new node();

    /* 这里是否可以优化 */
    /*
    uint32_t iter = 0;
    uint32_t i;
    for (i = 0; iter < static_cast<uint32_t>(std::ceil(BTREE_ORDER / 2.0));
         ++i) {
      child1->children[i] = node_in_overflow->children[iter];
      child1->data[i] = node_in_overflow->data[iter];
      ++child1->count;

      ++iter;
    }
    */

    uint32_t iter = static_cast<uint32_t>(std::ceil(BTREE_ORDER / 2.0));
    uint32_t i = iter;
    child1->count = iter; /* 更新count的计数, 析构以及其他操作都以此为准 */

    /*
     * iter位于split后右边节点的第一个位置, 比如3阶, 有4个元素
     * iter = 2, 此时data位于下半区第一个
     * iter位置的ptr在split时都要复制 line:139
     */
    assert(i == iter);
    /* 在pos处插入对应的key, 这里使用children下标恰好符合定义,
     * 因为分裂出来的节点下标比对应的key大1
     * 所以此处的pos恰好是原来key的下一个位置
     */
    ptr->insert_in_node(pos, node_in_overflow->data[iter]);

    if (node_in_overflow->children[0] != nullptr) {
      /*
       * 如果需要split的节点不是叶子节点, 节点值插入到他们的父亲节点中
       * child2不需要这个节点, 内部节点只有1个和leaf重复的key
       */
      ++iter;
    } else {
      /* 如果是叶子节点需要分裂 */
      child2->right = child1->right; /* 在叶子链表中插入child2 */
      child1->right = child2;        /* 叶子节点是一个有序链表 */
      /* 父亲节点的儿子需要被正确更新
       * 原来的children[pos]指向的是node_in_overflow
       * children[pos + 1]应该指向child2节点
       * 因为key[pos]等于child2的最小的(也是第一个)key
       */
      ptr->children[pos + 1] = child2; /* pos位置的右侧节点, 即≥ */
    }

    /* split节点, 将数据复制到child2中 */
    for (i = 0; iter < BTREE_ORDER + 1; ++i) {
      /*
       * child1的最后一个childptr和child2的第一个childptr指向同一个节点line:118
       */
      child2->children[i] = node_in_overflow->children[iter];
      child2->data[i] = node_in_overflow->data[iter];
      ++child2->count;
      ++iter;
    }

    /* children比key多1个, 这里需要正确复制最后一个指针 */
    child2->children[i] = node_in_overflow->children[iter];
    /* 更新父亲节点ptr对于child1和child2的指针 */
    ptr->children[pos] = child1;
    ptr->children[pos + 1] = child2;
  }

  /**
   * @brief btree需要增加高度
   *
   * @param ptr
   * @param val
   */
  void split_root(node *ptr, const T &val) {
    node *node_in_overflow = ptr;
    node *child1 = new node();
    node *child2 = new node();

    uint32_t iter = 0;
    uint32_t i;
    for (i = 0; i < static_cast<uint32_t>(std::ceil(BTREE_ORDER / 2.0)); ++i) {
      child1->children[i] = node_in_overflow->children[iter];
      child1->data[i] = node_in_overflow->data[iter];
      ++child1->count;

      ++iter;
    }

    assert(i == iter);
    child1->children[i] = node_in_overflow->children[iter];

    /* 中间节点向上提 */
    ptr->data[0] = node_in_overflow->data[iter];
    child1->right = child2;
    if (ptr->children[0] != nullptr) [[unlikely]] {
      /* 判断是否是内部节点, 如果是内部节点iter++,
       * child2不复制iter的key
       */
      ++iter;
    }
    /* 将溢出节点的key和children复制到child2中 */
    for (i = 0; iter < BTREE_ORDER + 1; ++i) {
      child2->children[i] = node_in_overflow->children[iter];
      child2->data[i] = node_in_overflow->data[iter];
      ++child2->count;

      ++iter;
    }

    child2->children[i] = node_in_overflow->children[iter];

    ptr->children[0] = child1;
    ptr->children[1] = child2;
    ptr->count = 1;
  }

  void remove(const T &val) {
    STATE state = remove(root, val);
    if (state == STATE::BT_UNDERFLOW && root->count == 0) {
      node *tmp = root;
      root = root->children[0]; /* 需要降低树的高度 */
      delete tmp;
    }
  }

  STATE remove(node *ptr, const T &val) {
    uint32_t pos = 0;
    /* 注意这里的下标, 如果是内部节点(children[pos] != nullptr)
     * 那么pos其实是基于children的下标, 传参到 delete_in_node
     * 时应该将 pos - 1 (因为chidren指针比key多1个)
     */
    while (pos < ptr->count && ptr->data[pos] < val) {
      ++pos; /* 找对应的子树 */
    }

    if (ptr->children[pos] != nullptr) {
      if (pos != ptr->count && ptr->data[pos] == val) {
        /*
         * 需要处理大于等于的情况:
         * 1 5 9
         * remove(5)的时候, pos == 0, 此时我们需要额外判断当pos == 1时的情况
         * 即data[1] == 5, 此时找到中序遍历的下一个节点
         * 另一种特殊情况是: remove(10)
         * 此时ptr->data[3] == 10(数组空间是足够的)但仍然不符合条件
         */
        ptr->data[pos] = succesor(ptr->children[pos + 1]);
        ++pos;
      }

      /* 在子树递归remove val */
      auto state = remove(ptr->children[pos], val);
      if (state == STATE::BT_UNDERFLOW) {
        /*
         * 解决方案:
         * 首先先判断是否能从兄弟节点steal一个key以满足定义(只有叶子节点可以)
         * 如果不行, 如果当前ptr在叶子节点的上一个level,
         * 试着合并叶子节点并释放空间
         * 如果依然不行, 那么我们则需要合并parent节点
         * 最后如果还是不行则需要降低树的高度
         */
        node *node_in_underflow = ptr->children[pos];
        bool can_steal = steal_sibling(node_in_underflow, ptr, pos);
        if (!can_steal) {
          if (ptr->children[pos]->children[0] == nullptr) {
            /*
             * 叶子节点的上一层才能merge_leaf, 同时这里的pos是children下标,
             * 并非key下标
             */
            merge_leaf(ptr, node_in_underflow, pos);
          } else {
            /* merge_with_parent 在叶子节点的上2层以及以上的层次发生,
             * 也就是说内部节点只出现一次, 不会存在重复的情况 */
            bool can_merge = merge_with_parent(ptr, node_in_underflow, pos);
            if (!can_merge)
              decrease_height(ptr, node_in_underflow, pos);
          }
        }
      }
    } else if (ptr->data[pos] == val) {
      /* 此分支表示当前ptr是叶子节点 */
      ptr->delete_in_node(pos);
    }

    return ptr->is_underflow() == true ? STATE::BT_UNDERFLOW : STATE::NORMAL;
  }

  /**
   * @brief 父节点ptr, 溢出节点 node_in_underflow
   * 试图通过ptr节点去给node_in_underflow向它的左右邻居(可能不存在)借节点
   *
   * @param node_in_underflow
   * @param ptr
   * @param pos
   * @return true
   * @return false
   */
  bool steal_sibling(node *node_in_underflow, node *ptr, uint32_t pos) {
    /* 只有当ptr在叶子节点的上一层时才能steal节点, 否则返回false */
    if (ptr->children[0]->children[0] == nullptr) {
      if (pos != ptr->count) {
        /* node_in_underflow 存在右兄弟,
         * 如果右兄弟借一个key也能满足b+树定义则执行操作 */
        if (ptr->children[pos + 1]->count - 1 >=
            static_cast<uint32_t>(std::floor(BTREE_ORDER / 2.0))) {
          // std::cout << "ptr->children[pos + 1]->count:steal_sibling\n";
          node *sibling = ptr->children[pos + 1]; /* 借右兄弟最小的key */
          T first = sibling->data[0];             /* 最小值 */
          T second = sibling->data[1];            /* 次小值 */
          sibling->delete_in_node(0);             /* 删除第一个节点 */
          node_in_underflow->insert_in_node(
              node_in_underflow->count,
              first); /* 插入末尾, 因为first大于node_in_underflow的所有值 */
          ptr->data[pos] = second;
          return true;
        }
      }

      if (pos > 0) {
        /* 如果能像左兄弟接也可以借一个key */
        if (ptr->children[pos - 1]->count - 1 >=
            static_cast<uint32_t>(std::floor(BTREE_ORDER / 2.0))) {
          // std::cout << "ptr->children[pos - 1]->count:steal_sibling\n";
          node *sibling = ptr->children[pos - 1];
          T rightmost = sibling->data[sibling->count -
                                      1]; /* 左兄弟的最大key(最后一个节点) */
          sibling->delete_in_node(sibling->count - 1); /* 删除最右侧的节点 */
          node_in_underflow->insert_in_node(
              0, rightmost); /* 前插, 因为小于当前所有节点 */
          ptr->data[pos - 1] =
              rightmost; /*rightmost被借到underflow中了, 更新节点信息*/
          return true;
        }
      }
    }

    return false;
  }

  T succesor(node *ptr) {
    /* 中序遍历的下一个节点 */
    while (ptr->children[0] != nullptr) {
      ptr = ptr->children[0];
    }
    if (ptr->count == 1) {
      if (ptr->right == nullptr)
        return -1; /* 应该返回一个T类型的不可能使用的key */
      /*
       * 是否有可能出现呢?
       * 可能: 3阶 插入1234
       *     3
       * 12    34
       * 删除4
       * 删除3
       * 因此我们的T应该预留一个不可能使用的值作为特殊标记
       * 这种情况会在删除时将父节点也处理掉, 所以不需要在意
       *
       */
      return ptr->right->data[0];
    } else {
      return ptr->data[1];
    }
  }

  /**
   * @brief 合并叶子节点
   * pos至少==1
   *
   */
  void merge_leaf(node *ptr, node *node_in_underflow, uint32_t pos) {
    if (pos >= 1) {
      /* 如果存在左边leaf */
      // std::cout << "pos - 1 >= 0 merge_leaf\n";
      node *sibling = ptr->children[pos - 1];
      for (uint32_t i = 0; i < node_in_underflow->count; ++i) {
        /* 将underflow的数据全部复制到sibling中, 删除underflow节点 */
        uint32_t pos_in = sibling->count;
        sibling->insert_in_node(pos_in, node_in_underflow->data[i]);
      }
      sibling->right = node_in_underflow->right; /* leaf链表的正确性 */
      ptr->delete_in_node(pos - 1);              /* 删除父节点 */
      /* 释放node_in_underflow的空间 */
      delete node_in_underflow; /* 释放node_in_underflow空间 */
    } else {
      /* pos == 0, 那么试图去合并右侧的叶子节点 */
      // std::cout << "pos == 0 merge_leaf\n";
      node *sibling = ptr->children[1];
      for (uint32_t i = 0; i < sibling->count; ++i) {
        int pos_in = node_in_underflow->count;
        /* 复制右侧节点到node_in_underflow */
        node_in_underflow->insert_in_node(pos_in, sibling->data[i]);
      }
      node_in_underflow->right = sibling->right;
      ptr->delete_in_node(
          0); /* 删除ptr的第一个节点, key[1]节点值一定大于children[1]的所有节点,
                 满足b+条件 */
      delete sibling; /* 释放sibling的空间 */
    }
  }

  /* 这里的pos也是children下标, merge_with_parent不能在叶子节点的上一层调用,
   * 必须再往上一层 */
  bool merge_with_parent(node *ptr, node *node_in_underflow, int pos) {
    /* 父亲节点下放到子节点, 左侧节点的最大值提升为parent节点 */
    if (pos != 0) {
      if ((ptr->children[pos - 1]->count - 1) >=
          static_cast<uint32_t>(std::floor(BTREE_ORDER / 2.0))) {
        // std::cout << "pos > 0 merge_with_parent\n";
        node *sibling = ptr->children[pos - 1];
        T first = ptr->data[pos - 1]; /* parent key */
        node_in_underflow->insert_in_node(
            0, first); /* 将parent的key插入到子节点中,前向插入, 因为更小 */
        ptr->data[pos - 1] =
            sibling->data[sibling->count - 1]; /* 晋升sibling最大节点 */

        node_in_underflow->children[0] = sibling->children[sibling->count];

        /* 删除sibling的最右边的哪个节点 */
        sibling->delete_in_node(sibling->count - 1);

        return true;
      }
    } else if (pos != BTREE_ORDER) {
      /* 找右侧节点做类似之前的操作 */
      if ((ptr->children[pos + 1]->count - 1) >=
          static_cast<uint32_t>(std::floor(BTREE_ORDER / 2.0))) {
        // std::cout << "pos != BTREE_ORDER  merge_with_parent\n";
        node *sibling = ptr->children[pos + 1];
        T next = ptr->data[pos]; /* 下一个children的key */
        /* 将next插入到 node_in_underflow 末尾 */
        // std::cout << "node_in_underflow->count: " << node_in_underflow->count
        // << '\n';
        /* node_in_underflow借用父节点的key[pos]并在末尾插入, 因为更大 */
        node_in_underflow->insert_in_node(node_in_underflow->count, next);
        /* 将兄弟的第一个key提升到父节点的key中 */
        ptr->data[pos] = sibling->data[0];

        /* 因为要移除sibling.key[0], 将它的左孩子移动到node_in_underflow中*/
        node_in_underflow->children[node_in_underflow->count] =
            sibling->children[0];

        /* sibling->children[1]的所有节点都小于sibling->key[1] */
        sibling->children[0] = sibling->children[1];

        sibling->delete_in_node(0); /* 删除第一个节点 */
        return true;
      }
    }

    return false;
  }

  void decrease_height(node *ptr, node *node_in_underflow, int pos) {
    if (pos != ptr->count) {
      /*
       * 不是最右侧的子树, 本质上就是合并children[pos] 和 children[pos + 1]
       * 两个孩子节点, 并删除children[pos]孩子
       */
      if (ptr->children[pos]->count <
          static_cast<uint32_t>(std::floor(BTREE_ORDER / 2.0))) {
        // std::cout << "decrease_height pos != ptr->count\n";
        /* 将data[pos]的数据下沉到右侧子节点 */

        /* node_in_underflow 右侧第一个兄弟 */
        node *sibling = ptr->children[pos + 1];
        /* 将ptr中的key下沉到sibling中 */
        sibling->insert_in_node(0, ptr->data[pos]);
        uint32_t last = node_in_underflow->count;

        /* 对于node_in_underflow节点来说, 它需要将所有的孩子都移交到sibling中 */
        sibling->children[0] = node_in_underflow->children[last];
        for (uint32_t i = last; i > 0; --i) {
          /* insert只保证>=key[pos]的children是正确的 */
          /* 所以要手动更新children[0] */
          sibling->insert_in_node(0, node_in_underflow->data[i - 1]);
          sibling->children[0] = node_in_underflow->children[i - 1];
        }

        ptr->delete_in_node(pos); /* ptr删除key[pos] */
        ptr->children[pos] = sibling;

        delete node_in_underflow; /* 释放 node_in_underflow的空间 */
        return;
      }
    }

    /*
     * 否则合并children[pos - 1] 和children[pos]两个孩子,
     * 并删除children[pos]节点
     */
    // std::cout << "decrease_height pos == ptr->count\n";
    node *sibling = ptr->children[pos - 1];
    uint32_t last = sibling->count;
    sibling->insert_in_node(last, ptr->data[pos - 1]);
    sibling->children[last + 1] = node_in_underflow->children[0];
    /* 将children[pos]的所有子节点移动到sibling中 */
    for (uint32_t i = 0; i < node_in_underflow->count; i++) {
      last = sibling->count;
      sibling->insert_in_node(last, node_in_underflow->data[i]);
      sibling->children[last + 1] = node_in_underflow->children[i + 1];
    }
    ptr->delete_in_node(pos - 1);
    assert(sibling == ptr->children[pos - 1]);
    ptr->children[pos - 1] = sibling; /* 其实children[pos - 1] == sibling */

    delete node_in_underflow; /* 释放node_in_underflow内存 */
  }

  void print() {
    print(root, 0);
    std::cout << "________________________\n";
  }

  void print(node *ptr, int level) {
    if (ptr) {
      int i;
      for (i = ptr->count - 1; i >= 0; i--) {
        print(ptr->children[i + 1], level + 1);

        for (int k = 0; k < level; k++) {
          std::cout << "    ";
        }
        std::cout << ptr->data[i] << "\n";
      }
      print(ptr->children[i + 1], level + 1);
    }
  }

private:
  node *root;
};
}; // namespace TinySql
#ifndef BLOCKS_VOXELTREE_H
#define BLOCKS_VOXELTREE_H

#include <cassert>
#include <cmath>
#include <cstdio>

template<typename T, unsigned char DS = 2>
class VoxelTree {
public:
    static_assert(DS > 1, "VoxelTree: division_size must be greater than 1!");

    VoxelTree(int levels, T &inital)
    {
        size_ = std::pow(DS, levels);
        root_ = new Node(LEAF);
        root_->leaf_set(inital);
    }
    ~VoxelTree()
    {
        delete root_;
    }

    const T &get(int x, int y, int z)
    {
        Node *node = root_;
        int size = size_;

        while(size != 0)
        {
            if(node->type() == BRANCH)
            {
                size /= DS;
                int x_ = x/size;
                int y_ = y/size;
                int z_ = z/size;
                node = node->branch_child(x_, y_, z_);
                x -= size * x_;
                y -= size * y_;
                z -= size * z_;
            } else {
                assert(node->type() == LEAF);
                return node->leaf_get();
            }
        }
        assert(false);
    }

    void set(int x, int y, int z, T &data)
    {
        Node *node = root_;
        int size = size_;

        while(size != 0)
        {
            if(node->type() == BRANCH)
            {
                size /= DS;
                int x_ = x/size;
                int y_ = y/size;
                int z_ = z/size;
                node = node->branch_child(x_, y_, z_);
                x -= size * x_;
                y -= size * y_;
                z -= size * z_;
            } else {
                assert(node->type() == LEAF);
                if(size == 1)
                {
                    node->leaf_set(data);
                    return;
                } else {
                    node->leaf_transform_branch();
                }
            }
        }
        assert(false);
    }

private:
    enum NodeType {LEAF, BRANCH};

    class Node {
    public:
        Node(NodeType type)
            :type_(type)
        {
            if(type == BRANCH)    init_branch();
            else if(type == LEAF) init_leaf();
            else assert(false);
        }

        Node() :Node(LEAF){}

        ~Node()
        {
            if(type_ == BRANCH)    cleanup_branch();
            else if(type_ == LEAF) cleanup_leaf();
            else assert(false);
        }

        NodeType type() {return type_;}

        void leaf_set(T &data) {assert(type_ == LEAF); data_.leaf.data = data;}
        T &leaf_get() {assert(type_ == LEAF); return data_.leaf.data;}

        void leaf_transform_branch()
        {
            assert(type_ == LEAF);
            Node tmp = Node(LEAF);
            tmp.leaf_set(data_.leaf.data);
            type_ = BRANCH;
            init_branch();
            for(int i=0; i<DS*DS*DS; i++)
                data_.branch.children[i] = tmp;
        }

        Node *branch_child(int x, int y, int z)
        {
            assert(x >= 0 && x < DS);
            assert(y >= 0 && y < DS);
            assert(z >= 0 && z < DS);

            return &data_.branch.children[x + DS*y + DS*DS*z];
        }

    private:
        void init_branch()
        {
            data_.branch.children = new Node[DS*DS*DS]();
        }

        void init_leaf()
        {
        }

        void cleanup_branch()
        {
            delete[] data_.branch.children;
        }

        void cleanup_leaf()
        {
        }

        union {
            struct {
                T data;
            } leaf;
            struct {
                Node *children;
            } branch;
        } data_;

        NodeType type_ : 1;
    };

    int size_;
    Node *root_;
};

#endif

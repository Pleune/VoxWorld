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
        root_ = new LeafNode(inital);
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
                BranchNode *bnode = reinterpret_cast<BranchNode *>(node);
                size /= DS;
                int x_ = x/size;
                int y_ = y/size;
                int z_ = z/size;
                node = *bnode->child(x_, y_, z_);
                x -= size * x_;
                y -= size * y_;
                z -= size * z_;
            } else {
                assert(node->type() == LEAF);
                LeafNode *lnode = reinterpret_cast<LeafNode *>(node);
                return lnode->get();
            }
        }
        assert(false);
    }

    void set(int x, int y, int z, T &data)
    {
        Node **node = &root_;
        int size = size_;

        while(size != 0)
        {
            if((*node)->type() == BRANCH)
            {
                BranchNode *bnode = reinterpret_cast<BranchNode *>(*node);
                size /= DS;
                int x_ = x/size;
                int y_ = y/size;
                int z_ = z/size;
                node = bnode->child(x_, y_, z_);
                x -= size * x_;
                y -= size * y_;
                z -= size * z_;
            } else {
                assert((*node)->type() == LEAF);
                LeafNode *lnode = reinterpret_cast<LeafNode *>(*node);
                if(size == 1)
                {
                    lnode->set(data);
                    return;
                } else {
                    *node = new BranchNode(lnode->get());
                    delete lnode;
                }
            }
        }
        assert(false);
    }

private:
    enum NodeType {LEAF, BRANCH};

    class Node {
    public:
        virtual ~Node() {}
        NodeType type() {return type_;}
    protected:
        Node(NodeType type) :type_(type) {};
        NodeType type_ : 1;
    };

    class LeafNode : public Node {
    public:
        LeafNode(T &data)
            :Node(LEAF),
             data_(data)
        {
        }
        void set(T &data) {data_ = data;}
        T &get() {return data_;}
    private:
        T data_;
    };

    class BranchNode : public Node {
    public:
        BranchNode(T &data)
            :Node(BRANCH)
        {
            for(int x=0; x<DS; x++)
            for(int y=0; y<DS; y++)
            for(int z=0; z<DS; z++)
                children[x][y][z] = new LeafNode(data);
        }
        ~BranchNode()
        {
            for(int x=0; x<DS; x++)
            for(int y=0; y<DS; y++)
            for(int z=0; z<DS; z++)
                delete children[x][y][z];
        }

        Node **child(int x, int y, int z)
        {
            assert(x >= 0 && x < DS);
            assert(y >= 0 && y < DS);
            assert(z >= 0 && z < DS);
            return &children[x][y][z];
        }

    private:
        Node *children[DS][DS][DS];
    };

    int size_;
    Node *root_;
};

#endif

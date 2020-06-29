#include "stdint.h"
#include "rbtree.h"
#include "malloc.h"

int cmp(KEY_TYPE a, KEY_TYPE b) {
    if(a[0] == b[0])
        return 0;
    return a[0] < b[0]? -1 : 1;
}

void init_tree(BR_Tree* tree) {
    tree->end = (Node)malloc(sizeof(struct _Node));
    tree->end->color = BLACK;
    tree->root = tree->end;
}

int insert_key(BR_Tree *tree, KEY_TYPE key) {
    Node e = (Node)malloc(sizeof(struct _Node));
    e->key = key;
    return insert_node(tree, e);
}

int delete_key(BR_Tree *tree, KEY_TYPE key) {
    Node temp = tree->root;
    int mark = 0;
    while(temp != tree->end && !mark) {
        int c = cmp(key,temp->key);
        switch(c) {
            case 0:
                mark = 1;
                delete_node(tree, temp);
                break;
            case -1:
                temp = temp->left;
                break;
            case 1:
                temp = temp->right;
                break;
        }
    }
    return 0;
}
KEY_TYPE find_key(BR_Tree *tree, KEY_TYPE key) {
    Node temp = tree->root;
    int mark = 0;
    KEY_TYPE ret = 0;
    while(temp != tree->end && !mark) {
        int c = cmp(key,temp->key);
        switch(c) {
            case 0:
                mark = 1;
                ret = temp->key;
                break;
            case -1:
                temp = temp->left;
                break;
            case 1:
                temp = temp->right;
                break;
        }
    }
    return ret;
}

static void delete_node(BR_Tree *tree, Node e) {
    Node y = e;
    Node x;
    int temp_color = y->color;
    if(e->left == tree->end) {
        x = e->right;
        trans_parent(tree, e, x);
    } else if(e->right == tree->end) {
        x = e->left;
        trans_parent(tree, e, x);
    } else {
        y = tree_min(tree, e->right);
        temp_color = y->color;
        x = y->right;
        if(y->p == e) {
            x->p=y;
        }else{
            trans_parent(tree, y, y->right);
            y->right = e->right;
            y->right->p = y;
        }
        trans_parent(tree, e, y);
        y->left = e->left;
        y->left->p = y;
        y->color = e->color;
    }
    if(temp_color == BLACK) {
        fixup_delete_node(tree, x);
    }
}
static void fixup_delete_node(BR_Tree *tree, Node x) {
    while(x != tree->root && x->color == BLACK) {
        if(x == x->p->left) {
            Node w = x->p->right;
            if(w->color == RED) {
                w->color = BLACK;
                x->p->color = RED;
                left_rotate(tree, x->p);
                w = x->p->right;
            }
            if(w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->p;
            } else {
                if(w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    right_rotate(tree, w);
                    w = x->p->right;
                }
                w->color = x->p->color;
                x->p->color = BLACK;
                w->right->color = BLACK;
                left_rotate(tree, x->p);
                x = tree->root;
            }
        } else {
            Node w = x->p->left;
            if(w->color == RED) {
                w->color = BLACK;
                x->p->color=RED;
                right_rotate(tree,x->p);
                w=x->p->left;
            }
            if(w->right->color == BLACK && w->left->color==BLACK){
                w->color=RED;
                x=x->p;
            }else{
                if(w->left->color==BLACK){
                    w->right->color=BLACK;
                    w->color=RED;
                    left_rotate(tree,w);
                    w=x->p->left;
                }
                w->color=x->p->color;
                x->p->color=BLACK;
                w->left->color=BLACK;
                right_rotate(tree,x->p);
                x=tree->root;
            }
        }
    }
    x->color = BLACK;
}

static int insert_node(BR_Tree *tree, Node e) {
    Node temp = tree->root;
    Node y = tree->end;
    while(temp != tree->end) {
        y = temp;
        if(e->key == temp->key) {
            return 0;
        }
        if(cmp(e->key, temp->key) == -1) {
            temp = temp->left;
        } else {
            temp = temp->right;
        }
    }
    e->p = y;
    e->left = e->right = tree->end;
    e->color = RED;
    if(y == tree->end) {
        tree->root = e;
    } else if(cmp(e->key, y->key) == -1) {
        y->left = e;
    } else {
        y->right = e;
    }
    fixup_insert_node(tree, e);
    return 1;
}

static void fixup_insert_node(BR_Tree *tree, Node e) {
    while(e->p->color == RED) {
        if(e->p == e->p->p->left) {
            Node u = e->p->p->right;
            if(u->color == RED) {
                e->p->color = BLACK;
                u->color = BLACK;
                e->p->p->color = RED;
                e = e->p->p;
            } else {
                if(e->p->right == e) {
                    e = e->p;
                    left_rotate(tree, e);
                }
                e->p->color = BLACK;
                e->p->p->color = RED;
                right_rotate(tree, e->p->p);
            }
        } else {
            Node u = e->p->p->left;
            if(u->color == RED) {
                e->p->color = BLACK;
                u->color = BLACK;
                e->p->p->color = RED;
                e = e->p->p;
            } else {
                if(e->p->left == e) {
                    e = e->p;
                    right_rotate(tree, e);
                }
                e->p->color = BLACK;
                e->p->p->color = RED;
                left_rotate(tree, e->p->p);
            }
        }
    }
    tree->root->color = BLACK;
}



static void left_rotate(BR_Tree* tree, Node e) {
    Node y = e->right;
    // move child
    e->right = y->left;
    if(y->left != tree->end) {
        y->left->p = e;
    }
    // set parent
    y->p = e->p;
    if(e->p == tree->end) {
        tree->root = y;
    } else if(e->p->left == e) {
        e->p->left = y;
    } else {
        e->p->right = y;
    }
    // end
    e->p = y;
    y->left = e;
}

static void right_rotate(BR_Tree* tree, Node e) {
    Node y = e->left;
    // move child
    e->left = y->right;
    if(y->right != tree->end) {
        y->right->p = e;
    }
    // set parent
    y->p = e->p;
    if(e->p == tree->end) {
        tree->root = y;
    } else if(e->p->left == e) {
        e->p->left = y;
    } else {
        e->p->right = y;
    }
    // end
    e->p = y;
    y->right = e;
}

static void trans_parent(BR_Tree* tree, Node a, Node b) {
    if(a->p == tree->end) {
        tree->root = b;
    } else if(a->p->left == a) {
        a->p->left = b;
    } else {
        a->p->right = b;
    }
    b->p = a->p;
}

static Node tree_min(BR_Tree* tree, Node a) {
    while(a->left != tree->end) {
        a = a->left;
    }
    return a;
}
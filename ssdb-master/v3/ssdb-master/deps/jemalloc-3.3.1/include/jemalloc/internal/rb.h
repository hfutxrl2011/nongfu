 

#ifndef RB_H_
#define	RB_H_

#if 0
__FBSDID("$FreeBSD: head/lib/libc/stdlib/rb.h 204493 2010-02-28 22:57:13Z jasone $");
#endif

#ifdef RB_COMPACT
 
#define	rb_node(a_type)							\
struct {								\
    a_type *rbn_left;							\
    a_type *rbn_right_red;						\
}
#else
#define	rb_node(a_type)							\
struct {								\
    a_type *rbn_left;							\
    a_type *rbn_right;							\
    bool rbn_red;							\
}
#endif

 
#define	rb_tree(a_type)							\
struct {								\
    a_type *rbt_root;							\
    a_type rbt_nil;							\
}

 
#define	rbtn_left_get(a_type, a_field, a_node)				\
    ((a_node)->a_field.rbn_left)
#define	rbtn_left_set(a_type, a_field, a_node, a_left) do {		\
    (a_node)->a_field.rbn_left = a_left;				\
} while (0)

#ifdef RB_COMPACT
 
#define	rbtn_right_get(a_type, a_field, a_node)				\
    ((a_type *) (((intptr_t) (a_node)->a_field.rbn_right_red)		\
      & ((ssize_t)-2)))
#define	rbtn_right_set(a_type, a_field, a_node, a_right) do {		\
    (a_node)->a_field.rbn_right_red = (a_type *) (((uintptr_t) a_right)	\
      | (((uintptr_t) (a_node)->a_field.rbn_right_red) & ((size_t)1)));	\
} while (0)

 
#define	rbtn_red_get(a_type, a_field, a_node)				\
    ((bool) (((uintptr_t) (a_node)->a_field.rbn_right_red)		\
      & ((size_t)1)))
#define	rbtn_color_set(a_type, a_field, a_node, a_red) do {		\
    (a_node)->a_field.rbn_right_red = (a_type *) ((((intptr_t)		\
      (a_node)->a_field.rbn_right_red) & ((ssize_t)-2))			\
      | ((ssize_t)a_red));						\
} while (0)
#define	rbtn_red_set(a_type, a_field, a_node) do {			\
    (a_node)->a_field.rbn_right_red = (a_type *) (((uintptr_t)		\
      (a_node)->a_field.rbn_right_red) | ((size_t)1));			\
} while (0)
#define	rbtn_black_set(a_type, a_field, a_node) do {			\
    (a_node)->a_field.rbn_right_red = (a_type *) (((intptr_t)		\
      (a_node)->a_field.rbn_right_red) & ((ssize_t)-2));		\
} while (0)
#else
 
#define	rbtn_right_get(a_type, a_field, a_node)				\
    ((a_node)->a_field.rbn_right)
#define	rbtn_right_set(a_type, a_field, a_node, a_right) do {		\
    (a_node)->a_field.rbn_right = a_right;				\
} while (0)

 
#define	rbtn_red_get(a_type, a_field, a_node)				\
    ((a_node)->a_field.rbn_red)
#define	rbtn_color_set(a_type, a_field, a_node, a_red) do {		\
    (a_node)->a_field.rbn_red = (a_red);				\
} while (0)
#define	rbtn_red_set(a_type, a_field, a_node) do {			\
    (a_node)->a_field.rbn_red = true;					\
} while (0)
#define	rbtn_black_set(a_type, a_field, a_node) do {			\
    (a_node)->a_field.rbn_red = false;					\
} while (0)
#endif

 
#define	rbt_node_new(a_type, a_field, a_rbt, a_node) do {		\
    rbtn_left_set(a_type, a_field, (a_node), &(a_rbt)->rbt_nil);	\
    rbtn_right_set(a_type, a_field, (a_node), &(a_rbt)->rbt_nil);	\
    rbtn_red_set(a_type, a_field, (a_node));				\
} while (0)

 
#define	rb_new(a_type, a_field, a_rbt) do {				\
    (a_rbt)->rbt_root = &(a_rbt)->rbt_nil;				\
    rbt_node_new(a_type, a_field, a_rbt, &(a_rbt)->rbt_nil);		\
    rbtn_black_set(a_type, a_field, &(a_rbt)->rbt_nil);			\
} while (0)

 
#define	rbtn_first(a_type, a_field, a_rbt, a_root, r_node) do {		\
    (r_node) = (a_root);						\
    if ((r_node) != &(a_rbt)->rbt_nil) {				\
	for (;								\
	  rbtn_left_get(a_type, a_field, (r_node)) != &(a_rbt)->rbt_nil;\
	  (r_node) = rbtn_left_get(a_type, a_field, (r_node))) {	\
	}								\
    }									\
} while (0)

#define	rbtn_last(a_type, a_field, a_rbt, a_root, r_node) do {		\
    (r_node) = (a_root);						\
    if ((r_node) != &(a_rbt)->rbt_nil) {				\
	for (; rbtn_right_get(a_type, a_field, (r_node)) !=		\
	  &(a_rbt)->rbt_nil; (r_node) = rbtn_right_get(a_type, a_field,	\
	  (r_node))) {							\
	}								\
    }									\
} while (0)

#define	rbtn_rotate_left(a_type, a_field, a_node, r_node) do {		\
    (r_node) = rbtn_right_get(a_type, a_field, (a_node));		\
    rbtn_right_set(a_type, a_field, (a_node),				\
      rbtn_left_get(a_type, a_field, (r_node)));			\
    rbtn_left_set(a_type, a_field, (r_node), (a_node));			\
} while (0)

#define	rbtn_rotate_right(a_type, a_field, a_node, r_node) do {		\
    (r_node) = rbtn_left_get(a_type, a_field, (a_node));		\
    rbtn_left_set(a_type, a_field, (a_node),				\
      rbtn_right_get(a_type, a_field, (r_node)));			\
    rbtn_right_set(a_type, a_field, (r_node), (a_node));		\
} while (0)

 

#define	rb_proto(a_attr, a_prefix, a_rbt_type, a_type)			\
a_attr void								\
a_prefix##new(a_rbt_type *rbtree);					\
a_attr a_type *								\
a_prefix##first(a_rbt_type *rbtree);					\
a_attr a_type *								\
a_prefix##last(a_rbt_type *rbtree);					\
a_attr a_type *								\
a_prefix##next(a_rbt_type *rbtree, a_type *node);			\
a_attr a_type *								\
a_prefix##prev(a_rbt_type *rbtree, a_type *node);			\
a_attr a_type *								\
a_prefix##search(a_rbt_type *rbtree, a_type *key);			\
a_attr a_type *								\
a_prefix##nsearch(a_rbt_type *rbtree, a_type *key);			\
a_attr a_type *								\
a_prefix##psearch(a_rbt_type *rbtree, a_type *key);			\
a_attr void								\
a_prefix##insert(a_rbt_type *rbtree, a_type *node);			\
a_attr void								\
a_prefix##remove(a_rbt_type *rbtree, a_type *node);			\
a_attr a_type *								\
a_prefix##iter(a_rbt_type *rbtree, a_type *start, a_type *(*cb)(	\
  a_rbt_type *, a_type *, void *), void *arg);				\
a_attr a_type *								\
a_prefix##reverse_iter(a_rbt_type *rbtree, a_type *start,		\
  a_type *(*cb)(a_rbt_type *, a_type *, void *), void *arg);

 
#define	rb_gen(a_attr, a_prefix, a_rbt_type, a_type, a_field, a_cmp)	\
a_attr void								\
a_prefix##new(a_rbt_type *rbtree) {					\
    rb_new(a_type, a_field, rbtree);					\
}									\
a_attr a_type *								\
a_prefix##first(a_rbt_type *rbtree) {					\
    a_type *ret;							\
    rbtn_first(a_type, a_field, rbtree, rbtree->rbt_root, ret);		\
    if (ret == &rbtree->rbt_nil) {					\
	ret = NULL;							\
    }									\
    return (ret);							\
}									\
a_attr a_type *								\
a_prefix##last(a_rbt_type *rbtree) {					\
    a_type *ret;							\
    rbtn_last(a_type, a_field, rbtree, rbtree->rbt_root, ret);		\
    if (ret == &rbtree->rbt_nil) {					\
	ret = NULL;							\
    }									\
    return (ret);							\
}									\
a_attr a_type *								\
a_prefix##next(a_rbt_type *rbtree, a_type *node) {			\
    a_type *ret;							\
    if (rbtn_right_get(a_type, a_field, node) != &rbtree->rbt_nil) {	\
	rbtn_first(a_type, a_field, rbtree, rbtn_right_get(a_type,	\
	  a_field, node), ret);						\
    } else {								\
	a_type *tnode = rbtree->rbt_root;				\
	assert(tnode != &rbtree->rbt_nil);				\
	ret = &rbtree->rbt_nil;						\
	while (true) {							\
	    int cmp = (a_cmp)(node, tnode);				\
	    if (cmp < 0) {						\
		ret = tnode;						\
		tnode = rbtn_left_get(a_type, a_field, tnode);		\
	    } else if (cmp > 0) {					\
		tnode = rbtn_right_get(a_type, a_field, tnode);		\
	    } else {							\
		break;							\
	    }								\
	    assert(tnode != &rbtree->rbt_nil);				\
	}								\
    }									\
    if (ret == &rbtree->rbt_nil) {					\
	ret = (NULL);							\
    }									\
    return (ret);							\
}									\
a_attr a_type *								\
a_prefix##prev(a_rbt_type *rbtree, a_type *node) {			\
    a_type *ret;							\
    if (rbtn_left_get(a_type, a_field, node) != &rbtree->rbt_nil) {	\
	rbtn_last(a_type, a_field, rbtree, rbtn_left_get(a_type,	\
	  a_field, node), ret);						\
    } else {								\
	a_type *tnode = rbtree->rbt_root;				\
	assert(tnode != &rbtree->rbt_nil);				\
	ret = &rbtree->rbt_nil;						\
	while (true) {							\
	    int cmp = (a_cmp)(node, tnode);				\
	    if (cmp < 0) {						\
		tnode = rbtn_left_get(a_type, a_field, tnode);		\
	    } else if (cmp > 0) {					\
		ret = tnode;						\
		tnode = rbtn_right_get(a_type, a_field, tnode);		\
	    } else {							\
		break;							\
	    }								\
	    assert(tnode != &rbtree->rbt_nil);				\
	}								\
    }									\
    if (ret == &rbtree->rbt_nil) {					\
	ret = (NULL);							\
    }									\
    return (ret);							\
}									\
a_attr a_type *								\
a_prefix##search(a_rbt_type *rbtree, a_type *key) {			\
    a_type *ret;							\
    int cmp;								\
    ret = rbtree->rbt_root;						\
    while (ret != &rbtree->rbt_nil					\
      && (cmp = (a_cmp)(key, ret)) != 0) {				\
	if (cmp < 0) {							\
	    ret = rbtn_left_get(a_type, a_field, ret);			\
	} else {							\
	    ret = rbtn_right_get(a_type, a_field, ret);			\
	}								\
    }									\
    if (ret == &rbtree->rbt_nil) {					\
	ret = (NULL);							\
    }									\
    return (ret);							\
}									\
a_attr a_type *								\
a_prefix##nsearch(a_rbt_type *rbtree, a_type *key) {			\
    a_type *ret;							\
    a_type *tnode = rbtree->rbt_root;					\
    ret = &rbtree->rbt_nil;						\
    while (tnode != &rbtree->rbt_nil) {					\
	int cmp = (a_cmp)(key, tnode);					\
	if (cmp < 0) {							\
	    ret = tnode;						\
	    tnode = rbtn_left_get(a_type, a_field, tnode);		\
	} else if (cmp > 0) {						\
	    tnode = rbtn_right_get(a_type, a_field, tnode);		\
	} else {							\
	    ret = tnode;						\
	    break;							\
	}								\
    }									\
    if (ret == &rbtree->rbt_nil) {					\
	ret = (NULL);							\
    }									\
    return (ret);							\
}									\
a_attr a_type *								\
a_prefix##psearch(a_rbt_type *rbtree, a_type *key) {			\
    a_type *ret;							\
    a_type *tnode = rbtree->rbt_root;					\
    ret = &rbtree->rbt_nil;						\
    while (tnode != &rbtree->rbt_nil) {					\
	int cmp = (a_cmp)(key, tnode);					\
	if (cmp < 0) {							\
	    tnode = rbtn_left_get(a_type, a_field, tnode);		\
	} else if (cmp > 0) {						\
	    ret = tnode;						\
	    tnode = rbtn_right_get(a_type, a_field, tnode);		\
	} else {							\
	    ret = tnode;						\
	    break;							\
	}								\
    }									\
    if (ret == &rbtree->rbt_nil) {					\
	ret = (NULL);							\
    }									\
    return (ret);							\
}									\
a_attr void								\
a_prefix##insert(a_rbt_type *rbtree, a_type *node) {			\
    struct {								\
	a_type *node;							\
	int cmp;							\
    } path[sizeof(void *) << 4], *pathp;				\
    rbt_node_new(a_type, a_field, rbtree, node);			\
     								\
    path->node = rbtree->rbt_root;					\
    for (pathp = path; pathp->node != &rbtree->rbt_nil; pathp++) {	\
	int cmp = pathp->cmp = a_cmp(node, pathp->node);		\
	assert(cmp != 0);						\
	if (cmp < 0) {							\
	    pathp[1].node = rbtn_left_get(a_type, a_field,		\
	      pathp->node);						\
	} else {							\
	    pathp[1].node = rbtn_right_get(a_type, a_field,		\
	      pathp->node);						\
	}								\
    }									\
    pathp->node = node;							\
     							\
    for (pathp--; (uintptr_t)pathp >= (uintptr_t)path; pathp--) {	\
	a_type *cnode = pathp->node;					\
	if (pathp->cmp < 0) {						\
	    a_type *left = pathp[1].node;				\
	    rbtn_left_set(a_type, a_field, cnode, left);		\
	    if (rbtn_red_get(a_type, a_field, left)) {			\
		a_type *leftleft = rbtn_left_get(a_type, a_field, left);\
		if (rbtn_red_get(a_type, a_field, leftleft)) {		\
		     				\
		    a_type *tnode;					\
		    rbtn_black_set(a_type, a_field, leftleft);		\
		    rbtn_rotate_right(a_type, a_field, cnode, tnode);	\
		    cnode = tnode;					\
		}							\
	    } else {							\
		return;							\
	    }								\
	} else {							\
	    a_type *right = pathp[1].node;				\
	    rbtn_right_set(a_type, a_field, cnode, right);		\
	    if (rbtn_red_get(a_type, a_field, right)) {			\
		a_type *left = rbtn_left_get(a_type, a_field, cnode);	\
		if (rbtn_red_get(a_type, a_field, left)) {		\
		     					\
		    rbtn_black_set(a_type, a_field, left);		\
		    rbtn_black_set(a_type, a_field, right);		\
		    rbtn_red_set(a_type, a_field, cnode);		\
		} else {						\
		     					\
		    a_type *tnode;					\
		    bool tred = rbtn_red_get(a_type, a_field, cnode);	\
		    rbtn_rotate_left(a_type, a_field, cnode, tnode);	\
		    rbtn_color_set(a_type, a_field, tnode, tred);	\
		    rbtn_red_set(a_type, a_field, cnode);		\
		    cnode = tnode;					\
		}							\
	    } else {							\
		return;							\
	    }								\
	}								\
	pathp->node = cnode;						\
    }									\
     					\
    rbtree->rbt_root = path->node;					\
    rbtn_black_set(a_type, a_field, rbtree->rbt_root);			\
}									\
a_attr void								\
a_prefix##remove(a_rbt_type *rbtree, a_type *node) {			\
    struct {								\
	a_type *node;							\
	int cmp;							\
    } *pathp, *nodep, path[sizeof(void *) << 4];			\
     								\
    nodep = NULL;  			\
    path->node = rbtree->rbt_root;					\
    for (pathp = path; pathp->node != &rbtree->rbt_nil; pathp++) {	\
	int cmp = pathp->cmp = a_cmp(node, pathp->node);		\
	if (cmp < 0) {							\
	    pathp[1].node = rbtn_left_get(a_type, a_field,		\
	      pathp->node);						\
	} else {							\
	    pathp[1].node = rbtn_right_get(a_type, a_field,		\
	      pathp->node);						\
	    if (cmp == 0) {						\
	         	\
		pathp->cmp = 1;						\
		nodep = pathp;						\
		for (pathp++; pathp->node != &rbtree->rbt_nil;		\
		  pathp++) {						\
		    pathp->cmp = -1;					\
		    pathp[1].node = rbtn_left_get(a_type, a_field,	\
		      pathp->node);					\
		}							\
		break;							\
	    }								\
	}								\
    }									\
    assert(nodep->node == node);					\
    pathp--;								\
    if (pathp->node != node) {						\
	 				\
	bool tred = rbtn_red_get(a_type, a_field, pathp->node);		\
	rbtn_color_set(a_type, a_field, pathp->node,			\
	  rbtn_red_get(a_type, a_field, node));				\
	rbtn_left_set(a_type, a_field, pathp->node,			\
	  rbtn_left_get(a_type, a_field, node));			\
	 \
	 \
	 \
	 \
	rbtn_right_set(a_type, a_field, pathp->node,			\
	  rbtn_right_get(a_type, a_field, node));			\
	rbtn_color_set(a_type, a_field, node, tred);			\
	 \
	 \
	nodep->node = pathp->node;					\
	pathp->node = node;						\
	if (nodep == path) {						\
	    rbtree->rbt_root = nodep->node;				\
	} else {							\
	    if (nodep[-1].cmp < 0) {					\
		rbtn_left_set(a_type, a_field, nodep[-1].node,		\
		  nodep->node);						\
	    } else {							\
		rbtn_right_set(a_type, a_field, nodep[-1].node,		\
		  nodep->node);						\
	    }								\
	}								\
    } else {								\
	a_type *left = rbtn_left_get(a_type, a_field, node);		\
	if (left != &rbtree->rbt_nil) {					\
	     \
	     \
	    assert(rbtn_red_get(a_type, a_field, node) == false);	\
	    assert(rbtn_red_get(a_type, a_field, left));		\
	    rbtn_black_set(a_type, a_field, left);			\
	    if (pathp == path) {					\
		rbtree->rbt_root = left;				\
	    } else {							\
		if (pathp[-1].cmp < 0) {				\
		    rbtn_left_set(a_type, a_field, pathp[-1].node,	\
		      left);						\
		} else {						\
		    rbtn_right_set(a_type, a_field, pathp[-1].node,	\
		      left);						\
		}							\
	    }								\
	    return;							\
	} else if (pathp == path) {					\
	     			\
	    rbtree->rbt_root = &rbtree->rbt_nil;			\
	    return;							\
	}								\
    }									\
    if (rbtn_red_get(a_type, a_field, pathp->node)) {			\
	 			\
	assert(pathp[-1].cmp < 0);					\
	rbtn_left_set(a_type, a_field, pathp[-1].node,			\
	  &rbtree->rbt_nil);						\
	return;								\
    }									\
     \
     \
    pathp->node = &rbtree->rbt_nil;					\
    for (pathp--; (uintptr_t)pathp >= (uintptr_t)path; pathp--) {	\
	assert(pathp->cmp != 0);					\
	if (pathp->cmp < 0) {						\
	    rbtn_left_set(a_type, a_field, pathp->node,			\
	      pathp[1].node);						\
	    assert(rbtn_red_get(a_type, a_field, pathp[1].node)		\
	      == false);						\
	    if (rbtn_red_get(a_type, a_field, pathp->node)) {		\
		a_type *right = rbtn_right_get(a_type, a_field,		\
		  pathp->node);						\
		a_type *rightleft = rbtn_left_get(a_type, a_field,	\
		  right);						\
		a_type *tnode;						\
		if (rbtn_red_get(a_type, a_field, rightleft)) {		\
		     \
		     \
		     \
		     \
		     \
		     \
		     \
		     \
		     \
		     \
		    rbtn_black_set(a_type, a_field, pathp->node);	\
		    rbtn_rotate_right(a_type, a_field, right, tnode);	\
		    rbtn_right_set(a_type, a_field, pathp->node, tnode);\
		    rbtn_rotate_left(a_type, a_field, pathp->node,	\
		      tnode);						\
		} else {						\
		     \
		     \
		     \
		     \
		     \
		     \
		     \
		    rbtn_rotate_left(a_type, a_field, pathp->node,	\
		      tnode);						\
		}							\
		 \
		 \
		assert((uintptr_t)pathp > (uintptr_t)path);		\
		if (pathp[-1].cmp < 0) {				\
		    rbtn_left_set(a_type, a_field, pathp[-1].node,	\
		      tnode);						\
		} else {						\
		    rbtn_right_set(a_type, a_field, pathp[-1].node,	\
		      tnode);						\
		}							\
		return;							\
	    } else {							\
		a_type *right = rbtn_right_get(a_type, a_field,		\
		  pathp->node);						\
		a_type *rightleft = rbtn_left_get(a_type, a_field,	\
		  right);						\
		if (rbtn_red_get(a_type, a_field, rightleft)) {		\
		     \
		     \
		     \
		     \
		     \
		     \
		    a_type *tnode;					\
		    rbtn_black_set(a_type, a_field, rightleft);		\
		    rbtn_rotate_right(a_type, a_field, right, tnode);	\
		    rbtn_right_set(a_type, a_field, pathp->node, tnode);\
		    rbtn_rotate_left(a_type, a_field, pathp->node,	\
		      tnode);						\
		     \
		     \
		     \
		    if (pathp == path) {				\
			 					\
			rbtree->rbt_root = tnode;			\
		    } else {						\
			if (pathp[-1].cmp < 0) {			\
			    rbtn_left_set(a_type, a_field,		\
			      pathp[-1].node, tnode);			\
			} else {					\
			    rbtn_right_set(a_type, a_field,		\
			      pathp[-1].node, tnode);			\
			}						\
		    }							\
		    return;						\
		} else {						\
		     \
		     \
		     \
		     \
		     \
		     \
		    a_type *tnode;					\
		    rbtn_red_set(a_type, a_field, pathp->node);		\
		    rbtn_rotate_left(a_type, a_field, pathp->node,	\
		      tnode);						\
		    pathp->node = tnode;				\
		}							\
	    }								\
	} else {							\
	    a_type *left;						\
	    rbtn_right_set(a_type, a_field, pathp->node,		\
	      pathp[1].node);						\
	    left = rbtn_left_get(a_type, a_field, pathp->node);		\
	    if (rbtn_red_get(a_type, a_field, left)) {			\
		a_type *tnode;						\
		a_type *leftright = rbtn_right_get(a_type, a_field,	\
		  left);						\
		a_type *leftrightleft = rbtn_left_get(a_type, a_field,	\
		  leftright);						\
		if (rbtn_red_get(a_type, a_field, leftrightleft)) {	\
		     \
		     \
		     \
		     \
		     \
		     \
		     \
		     \
		    a_type *unode;					\
		    rbtn_black_set(a_type, a_field, leftrightleft);	\
		    rbtn_rotate_right(a_type, a_field, pathp->node,	\
		      unode);						\
		    rbtn_rotate_right(a_type, a_field, pathp->node,	\
		      tnode);						\
		    rbtn_right_set(a_type, a_field, unode, tnode);	\
		    rbtn_rotate_left(a_type, a_field, unode, tnode);	\
		} else {						\
		     \
		     \
		     \
		     \
		     \
		     \
		     \
		     \
		    assert(leftright != &rbtree->rbt_nil);		\
		    rbtn_red_set(a_type, a_field, leftright);		\
		    rbtn_rotate_right(a_type, a_field, pathp->node,	\
		      tnode);						\
		    rbtn_black_set(a_type, a_field, tnode);		\
		}							\
		 \
		 \
		if (pathp == path) {					\
		     					\
		    rbtree->rbt_root = tnode;				\
		} else {						\
		    if (pathp[-1].cmp < 0) {				\
			rbtn_left_set(a_type, a_field, pathp[-1].node,	\
			  tnode);					\
		    } else {						\
			rbtn_right_set(a_type, a_field, pathp[-1].node,	\
			  tnode);					\
		    }							\
		}							\
		return;							\
	    } else if (rbtn_red_get(a_type, a_field, pathp->node)) {	\
		a_type *leftleft = rbtn_left_get(a_type, a_field, left);\
		if (rbtn_red_get(a_type, a_field, leftleft)) {		\
		     \
		     \
		     \
		     \
		     \
		     \
		    a_type *tnode;					\
		    rbtn_black_set(a_type, a_field, pathp->node);	\
		    rbtn_red_set(a_type, a_field, left);		\
		    rbtn_black_set(a_type, a_field, leftleft);		\
		    rbtn_rotate_right(a_type, a_field, pathp->node,	\
		      tnode);						\
		     \
		     \
		    assert((uintptr_t)pathp > (uintptr_t)path);		\
		    if (pathp[-1].cmp < 0) {				\
			rbtn_left_set(a_type, a_field, pathp[-1].node,	\
			  tnode);					\
		    } else {						\
			rbtn_right_set(a_type, a_field, pathp[-1].node,	\
			  tnode);					\
		    }							\
		    return;						\
		} else {						\
		     \
		     \
		     \
		     \
		     \
		     \
		    rbtn_red_set(a_type, a_field, left);		\
		    rbtn_black_set(a_type, a_field, pathp->node);	\
		     				\
		    return;						\
		}							\
	    } else {							\
		a_type *leftleft = rbtn_left_get(a_type, a_field, left);\
		if (rbtn_red_get(a_type, a_field, leftleft)) {		\
		     \
		     \
		     \
		     \
		     \
		     \
		    a_type *tnode;					\
		    rbtn_black_set(a_type, a_field, leftleft);		\
		    rbtn_rotate_right(a_type, a_field, pathp->node,	\
		      tnode);						\
		     \
		     \
		     \
		    if (pathp == path) {				\
			 					\
			rbtree->rbt_root = tnode;			\
		    } else {						\
			if (pathp[-1].cmp < 0) {			\
			    rbtn_left_set(a_type, a_field,		\
			      pathp[-1].node, tnode);			\
			} else {					\
			    rbtn_right_set(a_type, a_field,		\
			      pathp[-1].node, tnode);			\
			}						\
		    }							\
		    return;						\
		} else {						\
		     \
		     \
		     \
		     \
		     \
		     \
		    rbtn_red_set(a_type, a_field, left);		\
		}							\
	    }								\
	}								\
    }									\
     							\
    rbtree->rbt_root = path->node;					\
    assert(rbtn_red_get(a_type, a_field, rbtree->rbt_root) == false);	\
}									\
a_attr a_type *								\
a_prefix##iter_recurse(a_rbt_type *rbtree, a_type *node,		\
  a_type *(*cb)(a_rbt_type *, a_type *, void *), void *arg) {		\
    if (node == &rbtree->rbt_nil) {					\
	return (&rbtree->rbt_nil);					\
    } else {								\
	a_type *ret;							\
	if ((ret = a_prefix##iter_recurse(rbtree, rbtn_left_get(a_type,	\
	  a_field, node), cb, arg)) != &rbtree->rbt_nil			\
	  || (ret = cb(rbtree, node, arg)) != NULL) {			\
	    return (ret);						\
	}								\
	return (a_prefix##iter_recurse(rbtree, rbtn_right_get(a_type,	\
	  a_field, node), cb, arg));					\
    }									\
}									\
a_attr a_type *								\
a_prefix##iter_start(a_rbt_type *rbtree, a_type *start, a_type *node,	\
  a_type *(*cb)(a_rbt_type *, a_type *, void *), void *arg) {		\
    int cmp = a_cmp(start, node);					\
    if (cmp < 0) {							\
	a_type *ret;							\
	if ((ret = a_prefix##iter_start(rbtree, start,			\
	  rbtn_left_get(a_type, a_field, node), cb, arg)) !=		\
	  &rbtree->rbt_nil || (ret = cb(rbtree, node, arg)) != NULL) {	\
	    return (ret);						\
	}								\
	return (a_prefix##iter_recurse(rbtree, rbtn_right_get(a_type,	\
	  a_field, node), cb, arg));					\
    } else if (cmp > 0) {						\
	return (a_prefix##iter_start(rbtree, start,			\
	  rbtn_right_get(a_type, a_field, node), cb, arg));		\
    } else {								\
	a_type *ret;							\
	if ((ret = cb(rbtree, node, arg)) != NULL) {			\
	    return (ret);						\
	}								\
	return (a_prefix##iter_recurse(rbtree, rbtn_right_get(a_type,	\
	  a_field, node), cb, arg));					\
    }									\
}									\
a_attr a_type *								\
a_prefix##iter(a_rbt_type *rbtree, a_type *start, a_type *(*cb)(	\
  a_rbt_type *, a_type *, void *), void *arg) {				\
    a_type *ret;							\
    if (start != NULL) {						\
	ret = a_prefix##iter_start(rbtree, start, rbtree->rbt_root,	\
	  cb, arg);							\
    } else {								\
	ret = a_prefix##iter_recurse(rbtree, rbtree->rbt_root, cb, arg);\
    }									\
    if (ret == &rbtree->rbt_nil) {					\
	ret = NULL;							\
    }									\
    return (ret);							\
}									\
a_attr a_type *								\
a_prefix##reverse_iter_recurse(a_rbt_type *rbtree, a_type *node,	\
  a_type *(*cb)(a_rbt_type *, a_type *, void *), void *arg) {		\
    if (node == &rbtree->rbt_nil) {					\
	return (&rbtree->rbt_nil);					\
    } else {								\
	a_type *ret;							\
	if ((ret = a_prefix##reverse_iter_recurse(rbtree,		\
	  rbtn_right_get(a_type, a_field, node), cb, arg)) !=		\
	  &rbtree->rbt_nil || (ret = cb(rbtree, node, arg)) != NULL) {	\
	    return (ret);						\
	}								\
	return (a_prefix##reverse_iter_recurse(rbtree,			\
	  rbtn_left_get(a_type, a_field, node), cb, arg));		\
    }									\
}									\
a_attr a_type *								\
a_prefix##reverse_iter_start(a_rbt_type *rbtree, a_type *start,		\
  a_type *node, a_type *(*cb)(a_rbt_type *, a_type *, void *),		\
  void *arg) {								\
    int cmp = a_cmp(start, node);					\
    if (cmp > 0) {							\
	a_type *ret;							\
	if ((ret = a_prefix##reverse_iter_start(rbtree, start,		\
	  rbtn_right_get(a_type, a_field, node), cb, arg)) !=		\
	  &rbtree->rbt_nil || (ret = cb(rbtree, node, arg)) != NULL) {	\
	    return (ret);						\
	}								\
	return (a_prefix##reverse_iter_recurse(rbtree,			\
	  rbtn_left_get(a_type, a_field, node), cb, arg));		\
    } else if (cmp < 0) {						\
	return (a_prefix##reverse_iter_start(rbtree, start,		\
	  rbtn_left_get(a_type, a_field, node), cb, arg));		\
    } else {								\
	a_type *ret;							\
	if ((ret = cb(rbtree, node, arg)) != NULL) {			\
	    return (ret);						\
	}								\
	return (a_prefix##reverse_iter_recurse(rbtree,			\
	  rbtn_left_get(a_type, a_field, node), cb, arg));		\
    }									\
}									\
a_attr a_type *								\
a_prefix##reverse_iter(a_rbt_type *rbtree, a_type *start,		\
  a_type *(*cb)(a_rbt_type *, a_type *, void *), void *arg) {		\
    a_type *ret;							\
    if (start != NULL) {						\
	ret = a_prefix##reverse_iter_start(rbtree, start,		\
	  rbtree->rbt_root, cb, arg);					\
    } else {								\
	ret = a_prefix##reverse_iter_recurse(rbtree, rbtree->rbt_root,	\
	  cb, arg);							\
    }									\
    if (ret == &rbtree->rbt_nil) {					\
	ret = NULL;							\
    }									\
    return (ret);							\
}

#endif  

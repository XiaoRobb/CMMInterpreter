#ifndef CODE_H
#define CODE_H

#include "treenode.h"

class Code
{
public:
    Code(TreeNode * node,int type=0);
    TreeNode *nextNode;
    int type;//如果这条语句最后值为bool， 0:false 1:true 2:other
private:

};

#endif // CODE_H

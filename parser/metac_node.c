#include "metac_node.h"

static inline bool MetaCNode_IsExpr(metac_node_t node)
{
    return (node->Kind > node_exp_invalid && node->Kind < node_exp_max);
}

static inline bool MetaCNode_IsStatement(metac_node_t node)
{
    return (node->Kind > stmt_min && node->Kind < stmt_max);
}

static inline bool MetaCNode_IsDecl(metac_node_t node)
{
    return (node->Kind > decl_min && node->Kind < decl_max);
}

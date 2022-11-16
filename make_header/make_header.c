#include <stdio.h>

#include "../os/compat.h"
#include "../driver/metac_driver.c"
#include "../utils/read_file.c"
#include "../parser/metac_parser_obj.c"
#include "../semantic/metac_semantic_obj.c"
#include "../driver/metac_lpp.c"
#include "../codegen/metac_codegen.c"

#ifdef DEBUG_SERVER
#  include "../debug/debug_server.c"
#endif

int main(int argc, const char* argv[])
{
    metac_lpp_t lpp;
    DeclarationArray decls;
    metac_alloc_t alloc;

    Allocator_Init(&alloc, 0);
    /* lpp stands for lexer, preprocessor, parser */
    MetaCLPP_Init(&lpp, &alloc, 0);

    decls = ReadLexParse(argv[1], &lpp, 0);

    printf("Got %d declarations\n", (int32_t)decls.Length);
    {
        uint32_t i;
        metac_declaration_t* decl = 0;
        for(i = 0; i < decls.Length; i++)
        {
            decl = decls.Ptr[i];
            if (!decl)
            {
                continue;
            }

            MetaCPrinter_PrintForHeader(&lpp.Parser.DebugPrinter, decl);
        }

        printf("%.*s\n", (int) lpp.Parser.DebugPrinter.StringMemorySize, lpp.Parser.DebugPrinter.StringMemory);
    }

}

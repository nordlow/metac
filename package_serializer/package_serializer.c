#include "../compat.h"
#include "../utils/read_file.c"
#include "../cache/crc32.c"
#include "../metac_parser_obj.c"

#include <stdlib.h>
#include <stdio.h>

uint32_t EstimateNumberOfTokens(uint32_t length)
{
    uint32_t aligned_length = (length + 16) & ~15;
    float token_estimate = ( aligned_length / 4.7f );
    return (((uint32_t) token_estimate) + 128) & ~127;
}

bool errored = false;

static inline void LexFile(metac_lexer_t* lexer,
                           const char* path,
                           const char* text, uint32_t length)
{
    if (length)
    {
        uint32_t estimated =  EstimateNumberOfTokens( length );
        printf ("Estimated number of tokens: %d\n",
               estimated);
        if (estimated > 96 && estimated < 768)
            estimated = 1024;

        uint32_t fileHash = ~0;

        metac_lexer_state_t lexer_state =
            MetaCLexerStateFromBuffer(1, text, length);
        if (estimated > 96)
        {
            lexer->tokens = (metac_token_t*) malloc(estimated * sizeof(metac_token_t));
            lexer->tokens_capacity = estimated;
        }
        else
        {
            lexer->tokens = lexer->inlineTokens;
            lexer->tokens_capacity =
                sizeof(lexer->inlineTokens) / sizeof(lexer->tokens[0]);
        }

        while(length > 0)
        {
            uint32_t initialPosition = lexer_state.Position;

            metac_token_t token =
                *MetaCLexerLexNextToken(lexer, &lexer_state, text, length);
            uint32_t eaten_chars = lexer_state.Position - initialPosition;
            fileHash = crc32c(fileHash, text, eaten_chars);

            if (eaten_chars == 0)
            {
                printf("No chars consumed ... Token:%s\n", MetaCTokenEnum_toChars(token.TokenType));
                printf("String around the halt: : %.*s\n" ,20 , text - 10);
                errored = true;
                return ;
            }

            text += eaten_chars;
            length -= eaten_chars;
        }

        printf("Lexed %d tokens\n", (int) lexer->tokens_size);
    }
}
const char** includePaths = 0;
uint32_t includePathCount = 0;
uint32_t includePathCapacity = 0;

void AddIncludePath(const char* path)
{
    assert(includePathCount < includePathCapacity);

    *(includePaths + includePathCount++) = path;
}

int main(int argc, const char* argv[])
{
    includePathCount = 0;
    includePathCapacity = 256;
    includePaths = malloc(sizeof(char**) * includePathCapacity);
    const char* arg = "bigcode.c";

    for(int arg_idx = 1;
        arg_idx < argc;
        arg_idx++)
    {
        arg = argv[arg_idx];
        if (arg[0] == '-')
        {
            if (arg[1] == 'I')
            {
                AddIncludePath(arg + 2);
            }
            else
            {
                fprintf(stderr, "Unkown option: %s", arg);
            }
            continue;
        }
        printf("arg: %s\n", arg);
        metac_lexer_t lexer;
        MetaCLexerInit(&lexer);

        read_result_t readResult = ReadFileAndZeroTerminate(arg);

        LexFile(&lexer, arg,
            readResult.FileContent0, readResult.FileLength

        );
#ifndef NO_DUMP
#if ACCEL == ACCEL_TABLE
        char formatBuffer[512];
        sprintf(formatBuffer, "%s.identifiers", arg);
        FILE* fd = fopen(formatBuffer, "wb");
        WriteIdentifiers(&lexer.IdentifierTable, fd);
        sprintf(formatBuffer, "%s.strings", arg);
        fd = fopen(formatBuffer, "wb");
        WriteIdentifiers(&lexer.StringTable, fd);
#endif
#endif
        metac_parser_t parser;
        MetaCParserInitFromLexer(&parser, &lexer);
    }
    return errored;
}

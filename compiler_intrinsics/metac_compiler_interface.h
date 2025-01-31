#ifndef _METAC_COMPILER_INTERFACE_H_
#define _METAC_COMPILER_INTERFACE_H_

typedef int int32_t;
typedef unsigned int uint32_t;
typedef unsigned long int uint64_t;

#ifndef __METAC__
typedef void* type;
#endif

typedef enum type_kind_t
{
    TypeKind_Invalid,
    TypeKind_Struct = 5,
    TypeKind_Class = TypeKind_Struct + 2,
    TypeKind_Max
} type_kind_t;

typedef struct metac_compiler_t
{
    void* semanticState;

    uint32_t StartTimeStamp;
    void (*CurrentTimeStamp) (uint32_t* outResult);
    void (*BuiltinCount) (uint32_t* outResult);

    const char* (*PrintType) (struct metac_compiler_t* compilerP, type* T);

    const char** (*FieldNames) (type* T);

    const char* (*BuiltinName) (uint32_t* builtinNumber);
    const char* (*Help) ();

    void (*Message) (struct metac_compiler_t* compilerP,
                     const char* str,
                     void* context);

    void (*Error) (struct metac_compiler_t* compilerP,
                   const char* str,
                   void* context);

    type_kind_t* (*GetTypeKind) (struct metac_compiler_t* compilerP, type* T);

    void (*RegisterLogCallback) (struct metac_compiler_t* compilerP,
                                 void (*LogCb)(const char* msg, void* context),
                                 void* context);

    void (*RegisterIdentifierCallback) (struct metac_compiler_t* compilerP,
                                        void (*IdentfierCb)(const char* idChars, uint32_t idKey, void* context),
                                        void* context);
    void (*PrintInt) (int32_t* value);
} metac_compiler_t;

#endif


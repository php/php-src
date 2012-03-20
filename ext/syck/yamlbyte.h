/*  yamlbyte.h
 *
 *  The YAML bytecode "C" interface header file.   See the YAML bytecode
 *  reference for bytecode sequence rules and for the meaning of each
 *  bytecode.
 */

#ifndef YAMLBYTE_H
#define YAMLBYTE_H
#include <stddef.h>

/* define what a character is */
typedef unsigned char yamlbyte_utf8_t;
typedef unsigned short yamlbyte_utf16_t;
#ifdef YAMLBYTE_UTF8
  #ifdef YAMLBYTE_UTF16
    #error Must only define YAMLBYTE_UTF8 or YAMLBYTE_UTF16
  #endif
  typedef yamlbyte_utf8_t yamlbyte_char_t;
#else
  #ifdef YAMLBYTE_UTF16
    typedef yamlbyte_utf16_t yamlbyte_char_t;
  #else
    #error Must define YAMLBYTE_UTF8 or YAMLBYTE_UTF16
  #endif
#endif

/* specify list of bytecodes */
#define YAMLBYTE_FINISH          ((yamlbyte_char_t) 0)
#define YAMLBYTE_DOCUMENT        ((yamlbyte_char_t)'D')
#define YAMLBYTE_DIRECTIVE       ((yamlbyte_char_t)'V')
#define YAMLBYTE_PAUSE           ((yamlbyte_char_t)'P')
#define YAMLBYTE_MAPPING         ((yamlbyte_char_t)'M')
#define YAMLBYTE_SEQUENCE        ((yamlbyte_char_t)'Q')
#define YAMLBYTE_END_BRANCH      ((yamlbyte_char_t)'E')
#define YAMLBYTE_SCALAR          ((yamlbyte_char_t)'S')
#define YAMLBYTE_CONTINUE        ((yamlbyte_char_t)'C')
#define YAMLBYTE_NEWLINE         ((yamlbyte_char_t)'N')
#define YAMLBYTE_NULLCHAR        ((yamlbyte_char_t)'Z')
#define YAMLBYTE_ANCHOR          ((yamlbyte_char_t)'A')
#define YAMLBYTE_ALIAS           ((yamlbyte_char_t)'R')
#define YAMLBYTE_TRANSFER        ((yamlbyte_char_t)'T')
/* formatting bytecodes */
#define YAMLBYTE_COMMENT         ((yamlbyte_char_t)'c')
#define YAMLBYTE_INDENT          ((yamlbyte_char_t)'i')
#define YAMLBYTE_STYLE           ((yamlbyte_char_t)'s')
/* other bytecodes */
#define YAMLBYTE_LINE_NUMBER     ((yamlbyte_char_t)'#')
#define YAMLBYTE_WHOLE_SCALAR    ((yamlbyte_char_t)'<')
#define YAMLBYTE_NOTICE          ((yamlbyte_char_t)'!')
#define YAMLBYTE_SPAN            ((yamlbyte_char_t)')')
#define YAMLBYTE_ALLOC           ((yamlbyte_char_t)'@')

/* second level style bytecodes, ie "s>" */
#define YAMLBYTE_FLOW            ((yamlbyte_char_t)'>')
#define YAMLBYTE_LITERAL         ((yamlbyte_char_t)'|')
#define YAMLBYTE_BLOCK           ((yamlbyte_char_t)'b')
#define YAMLBYTE_PLAIN           ((yamlbyte_char_t)'p')
#define YAMLBYTE_INLINE_MAPPING  ((yamlbyte_char_t)'{')
#define YAMLBYTE_INLINE_SEQUENCE ((yamlbyte_char_t)'[')
#define YAMLBYTE_SINGLE_QUOTED   ((yamlbyte_char_t)39)
#define YAMLBYTE_DOUBLE_QUOTED   ((yamlbyte_char_t)'"')

/*
 * The "C" API has two variants, one based on instructions,
 * with events delivered via pointers; and the other one
 * is character based where one or more instructions are
 * serialized into a buffer.
 *
 * Note: In the instruction based API, WHOLE_SCALAR does
 *       not have the '<here' marshalling stuff.
 */

typedef void * yamlbyte_consumer_t;
typedef void * yamlbyte_producer_t;

/* push and pull APIs need a way to communicate results */
typedef enum {
    YAMLBYTE_OK          = 0,     /* proceed                        */
    YAMLBYTE_E_MEMORY    = 'M',   /* could not allocate memory      */
    YAMLBYTE_E_READ      = 'R',   /* input stream read error        */
    YAMLBYTE_E_WRITE     = 'W',   /* output stream write error      */
    YAMLBYTE_E_OTHER     = '?',   /* some other error condition     */
    YAMLBYTE_E_PARSE     = 'P',   /* parse error, check bytecodes   */
    YAMLBYTE_MAX
} yamlbyte_result_t;

typedef const yamlbyte_char_t *yamlbyte_buff_t;

/*
 *  The "Instruction" API
 */

typedef struct yaml_instruction {
    yamlbyte_char_t bytecode;
    yamlbyte_buff_t start;
    yamlbyte_buff_t finish;  /* open range, *finish is _not_ part */
} *yamlbyte_inst_t;

/* producer pushes the instruction with one bytecode event to the
 * consumer; if the consumer's result is not YAMLBYTE_OK, then
 * the producer should stop */
typedef
  yamlbyte_result_t
   (*yamlbyte_push_t)(
     yamlbyte_consumer_t self,
     yamlbyte_inst_t  inst
   );

/* consumer pulls a bytecode instruction from the producer; in this
 * case the instruction (and is buffer) are owned by the producer and
 * will remain valid till the pull function is called once again;
 * if the instruction is NULL, then there are no more results; and
 * it is important to call the pull function till it returns NULL so
 * that the producer can clean up its memory allocations */
typedef
   yamlbyte_result_t
    (*yamlbyte_pull_t)(
      yamlbyte_producer_t self,
      yamlbyte_inst_t *inst   /* to be filled in by the producer */
    );

/*
 *  Buffer based API
 */

/* producer pushes a null terminated buffer filled with one or more
 * bytecode events to the consumer; if the consumer's result is not
 * YAMLBYTE_OK, then the producer should stop */
typedef
  yamlbyte_result_t
   (*yamlbyte_pushbuff_t)(
     yamlbyte_consumer_t self,
     yamlbyte_buff_t  buff
   );

/* consumer pulls bytecode events from the producer; in this case
 * the buffer is owned by the producer, and will remain valid till
 * the pull function is called once again; if the buffer pointer
 * is set to NULL, then there are no more results; it is important
 * to call the pull function till it returns NULL so that the
 * producer can clean up its memory allocations */
typedef
   yamlbyte_result_t
    (*yamlbyte_pullbuff_t)(
      yamlbyte_producer_t self,
      yamlbyte_buff_t *buff   /* to be filled in by the producer */
    );

/* convert a pull interface to a push interface; the reverse process
 * requires threads and thus is language dependent */
#define YAMLBYTE_PULL2PUSH(pull,producer,push,consumer,result)       \
    do {                                                         \
        yamlbyte_pullbuff_t _pull = (pull);                              \
        yamlbyte_pushbuff_t _push = (push);                              \
        yamlbyte_result_t _result = YAMLBYTE_OK;                         \
        yamlbyte_producer_t _producer = (producer);                  \
        yamlbyte_consumer_t _consumer = (consumer);                  \
        while(1) {                                               \
            yamlbyte_buff_t buff = NULL;                           \
            _result = _pull(_producer,&buff);                    \
            if(YAMLBYTE_OK != result || NULL == buff)                \
                break;                                           \
            _result = _push(_consumer,buff);                     \
            if(YAMLBYTE_OK != result)                                \
                break;                                           \
        }                                                        \
        (result) = _result;                                      \
    } while(0)

#endif

#####################
 High-level overview
#####################

PHP is an interpreted language. Interpreted languages differ from compiled ones in that they aren't
compiled into machine-readable code ahead of time. Instead, the source files are read, processed and
interpreted when the program is executed. This can be very convenient for developers for rapid
prototyping, as it skips a lengthy compilation phase. However, it also poses some unique challenges
to performance, which is one of the primary reasons interpreters can be complex. php-src borrows
many concepts from other compilers and interpreters.

**********
 Pipeline
**********

The goal of the interpreter is to read the users source files, and to simulate the users intent.
This process can be split into distinct phases that are easier to understand and implement.

-  Tokenization - splitting whole source files into words, called tokens.
-  Parsing - building a tree structure from tokens, called AST (abstract syntax tree).
-  Compilation - traversing the AST and building a list of operations, called opcodes.
-  Interpretation - reading and executing opcodes.

php-src as a whole can be seen as a pipeline consisting of these stages, using the input of the
previous phase and producing some output for the next.

.. code:: haskell

   source_code
     |> tokenizer   -- tokens
     |> parser      -- ast
     |> compiler    -- opcodes
     |> interpreter

Let's go into each phase in a bit more detail.

**************
 Tokenization
**************

Tokenization, often called "lexing" or "scanning", is the process of taking an entire program file
and splitting it into a list of words and symbols. Tokens generally consist of a type, a simple
integer constant representing the token, and a lexeme, the literal string used in the source code.

.. code:: php

   if ($cond) {
       echo "Cond is true\n";
   }

.. code:: text

   T_IF                       "if"
   T_WHITESPACE               " "
                              "("
   T_VARIABLE                 "$cond"
                              ")"
   T_WHITESPACE               " "
                              "{"
   T_WHITESPACE               "\n    "
   T_ECHO                     "echo"
   T_WHITESPACE               " "
   T_CONSTANT_ENCAPSED_STRING '"Cond is true\n"'
                              ";"
   T_WHITESPACE               "\n"
                              "}"

While tokenizers are not difficult to write by hand, PHP uses a tool called ``re2c`` to automate
this process. It takes a definition file and generates efficient C code to build these tokens from a
stream of characters. The definition for PHP lives in ``Zend/zend_language_scanner.l``. Check the
`re2c documentation`_ for details.

.. _re2c documentation: https://re2c.org/

*********
 Parsing
*********

Parsing is the process of reading the tokens generated from the tokenizer and building a tree
structure from it. To humans, how source code elements are grouped seems obvious through whitespace
and the usage of symbols like ``()`` and ``{}``. However, computers cannot visually glance over the
code to determine these boundaries quickly. To make it easier and faster to work with, we build a
tree structure from the tokens to more closely reflect the source code the way humans see it.

Here is a simplified example of what an AST from the tokens above might look like.

.. code:: text

   ZEND_AST_IF {
       ZEND_AST_IF_ELEM {
           ZEND_AST_VAR {
               ZEND_AST_ZVAL { "cond" },
           },
           ZEND_AST_STMT_LIST {
               ZEND_AST_ECHO {
                   ZEND_AST_ZVAL { "Cond is true\n" },
               },
           },
       },
   }

Each AST node has a type and may have children. They also store their original position in the
source code, and may define some arbitrary flags. These are omitted for brevity.

Like with tokenization, we use a tool called ``Bison`` to generate the parser implementation from a
grammar specification. The grammar lives in the ``Zend/zend_language_parser.y`` file. Check the
`Bison documentation`_ for details. Luckily, the syntax is quite approachable.

.. _bison documentation: https://www.gnu.org/software/bison/manual/

Parsing is described in more detail in its `dedicated chapter <todo>`__.

*************
 Compilation
*************

Computers don't understand human language, or even programming languages. They only understand
machine code, which are sequences of simple, mostly atomic instructions for doing one thing. For
example, they may add two numbers, load some memory from RAM, jump to an instruction under a certain
condition, etc. It turns out that even the most complex expressions can be reduced to a number of
these simple instructions.

PHP is a bit different, in that it does not execute machine code directly. Instead, instructions run
on a "virtual machine", often abbreviated to VM. This is just a fancy way of saying that there is no
physical machine you can buy that understands these instructions, but that this machine is
implemented in software. This is our interpreter. This also means that we are free to make up
instructions ourselves at will. Some of these instructions look very similar to something you'd find
in an actual CPU instruction set (e.g. adding two numbers), while others are much more high-level
(e.g. load property of object by name).

With that little detour out of the way, the job of the compiler is to read the AST and translate it
into our virtual machine instructions, also called opcodes. The code responsible for this
transformation lives in ``Zend/zend_compile.c``. It essentially traverses the AST and generates a
number of instructions, before going to the next node.

Here's what the surprisingly compact opcodes for the AST above might look like:

.. code:: text

   0000 JMPZ CV0($cond) 0002
   0001 ECHO string("Cond is true\n")
   0002 RETURN int(1)

****************
 Interpretation
****************

Finally, the opcodes are read and executed by the interpreter. PHPs uses `three-address code`_ for
instructions. This essentially means that each instructions may have a result value, and at most two
operands. Most modern CPUs also use this format. Both result and operands in PHP are :doc:`zvals
<../core/data-structures/zval>`.

.. _three-address code: https://en.wikipedia.org/wiki/Three-address_code

How exactly each opcode behaves depends on its purpose. You can find a complete list of opcodes in
the generated ``Zend/zend_vm_opcodes.h`` file. The behavior of each instruction is defined in
``Zend/zend_vm_def.h``.

Let's step through the opcodes form the example above:

-  We start at the top, i.e. ``JMPZ``. If its first operand contains a "falsy" value, it will jump
   to the instruction encoded in its second operand. If it is truthy, it will simply fall-through to
   the next instruction.

-  The ``ECHO`` instruction prints its first operand.

-  The ``RETURN`` operand terminates the current function.

With these simple rules, we can see that the interpreter will ``echo`` only when ``$cond`` is
truthy, and skip over the ``echo`` otherwise.

That's it! This is how PHP works, fundamentally. Of course, we skipped over a ton of details. The VM
is quite complex, and will be discussed separately in the `virtual machine <todo>`__ chapter.

*********
 Opcache
*********

As you may imagine, running this whole pipeline every time PHP serves a request is time consuming.
Luckily, it is also not necessary. We can cache the opcodes in memory between requests, to skip over
all of the phases, except for the execution phase. This is precisely what the opcache extension
does. It lives in the ``ext/opcache`` directory.

Opcache also performs some optimizations on the opcodes before caching them. As opcaches are
expected to be reused many times, it is profitable to spend some additional time simplifying them if
possible to improve performance during execution. The optimizer lives in ``Zend/Optimizer``.

JIT
===

The opcache also implements a JIT compiler, which stands for just-in-time compiler. This compiler
takes the virtual PHP opcodes and turns it into actual machine instructions, with additional
information gained at runtime. JITs are very complex pieces of software, so this book will likely
barely scratch the surface of how it works. It lives in ``ext/opcache/jit``.

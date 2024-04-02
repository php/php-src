##############
 php-src docs
##############

.. toctree::
   :caption: Introduction
   :hidden:

   introduction/high-level-overview
   introduction/ides/index

.. toctree::
   :caption: Core
   :hidden:

   core/data-structures/index

Welcome to the php-src documentation!

.. warning::

   This documentation is work in progress.

   At this point in time, there are other guides that provide a more complete picture of the PHP
   project. Check the `CONTRIBUTING.md
   <https://github.com/php/php-src/blob/master/CONTRIBUTING.md#technical-resources>`__ file for a
   list of technical resources.

php-src is the canonical implementation of the interpreter for the PHP programming language, as well
as various extensions that provide common functionality. This documentation is intended to help you
understand how the interpreter works, how you can build and test changes, and how you can create
extensions yourself.

This documentation is not intended to be comprehensive, but is meant to explain core concepts that
are not easy to grasp by reading code alone. It describes best practices, and will frequently omit
APIs that are discouraged for general use.

******************
 How to get help?
******************

Getting started with a new and complicated project like php-src can be overwhelming. While there's
no way around reading lots and lots of code, asking questions of somebody with experience can save a
lot of time. Luckily, many core developers are eager to help. Here are some ways you can get in
touch.

-  `Discord <https://phpc.chat>`__ (``#php-internals`` channel)
-  `R11 on StackOverflow <https://chat.stackoverflow.com/rooms/11/php>`__

***************
 Prerequisites
***************

The php-src interpreter is written in C, and so are most of the bundled extensions. While extensions
may also be written in C++, ext-intl is currently the only bundled extension to do so. It is
advisable that you have *some* knowledge of C before jumping into php-src.

It is also advisable to get familiar with the semantics of PHP itself, so that you may better
differentiate between bugs and expected behavior, and model new language features.

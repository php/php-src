require 'rdoc'

##
# RDoc uses generators to turn parsed source code in the form of an
# RDoc::CodeObject tree into some form of output.  RDoc comes with the HTML
# generator RDoc::Generator::Darkfish and an ri data generator
# RDoc::Generator::RI.
#
# = Registering a Generator
#
# Generators are registered by calling RDoc::RDoc.add_generator with the class
# of the generator:
#
#   class My::Awesome::Generator
#     RDoc::RDoc.add_generator self
#   end
#
# = Adding Options to +rdoc+
#
# Before option processing in +rdoc+, RDoc::Options will call ::setup_options
# on the generator class with an RDoc::Options instance.  The generator can
# use RDoc::Options#option_parser to add command-line options to the +rdoc+
# tool.  See OptionParser for details on how to add options.
#
# You can extend the RDoc::Options instance with additional accessors for your
# generator.
#
# = Generator Instantiation
#
# After parsing, RDoc::RDoc will instantiate a generator by calling
# #initialize with an RDoc::Options instance.
#
# RDoc will then call #generate on the generator instance and pass in an Array
# of RDoc::TopLevel instances, each representing a parsed file.  You can use
# the various class methods on RDoc::TopLevel and in the RDoc::CodeObject tree
# to create your desired output format.

module RDoc::Generator
end


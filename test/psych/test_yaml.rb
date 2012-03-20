# -*- mode: ruby; ruby-indent-level: 4; tab-width: 4 -*-
#												vim:sw=4:ts=4
# $Id$
#
require 'psych/helper'
require 'ostruct'

# [ruby-core:01946]
module Psych_Tests
    StructTest = Struct::new( :c )
end

class Psych_Unit_Tests < Psych::TestCase
    def teardown
        Psych.domain_types.clear
    end

    def test_y_method
      assert_raises(NoMethodError) do
        OpenStruct.new.y 1
      end
    end

    def test_syck_compat
      time = Time.utc(2010, 10, 10)
      yaml = Psych.dump time
      assert_match "2010-10-10 00:00:00.000000000 Z", yaml
    end

    # [ruby-core:34969]
    def test_regexp_with_n
        assert_cycle(Regexp.new('',0,'n'))
    end
	#
	# Tests modified from 00basic.t in Psych.pm
	#
	def test_basic_map
		# Simple map
		assert_parse_only(
			{ 'one' => 'foo', 'three' => 'baz', 'two' => 'bar' }, <<EOY
one: foo
two: bar
three: baz
EOY
		)
	end

	def test_basic_strings
		# Common string types
		assert_cycle("x")
		assert_cycle(":x")
		assert_cycle(":")
		assert_parse_only(
			{ 1 => 'simple string', 2 => 42, 3 => '1 Single Quoted String',
			  4 => 'Psych\'s Double "Quoted" String', 5 => "A block\n  with several\n    lines.\n",
			  6 => "A \"chomped\" block", 7 => "A folded\n string\n", 8 => ": started string" },
			  <<EOY
1: simple string
2: 42
3: '1 Single Quoted String'
4: "Psych's Double \\\"Quoted\\\" String"
5: |
  A block
    with several
      lines.
6: |-
  A "chomped" block
7: >
  A
  folded
   string
8: ": started string"
EOY
		)
	end

	#
	# Test the specification examples
	# - Many examples have been changes because of whitespace problems that
	#   caused the two to be inequivalent, or keys to be sorted wrong
	#

	def test_spec_simple_implicit_sequence
	  	# Simple implicit sequence
		assert_to_yaml(
			[ 'Mark McGwire', 'Sammy Sosa', 'Ken Griffey' ], <<EOY
- Mark McGwire
- Sammy Sosa
- Ken Griffey
EOY
		)
	end

	def test_spec_simple_implicit_map
		# Simple implicit map
		assert_to_yaml(
			{ 'hr' => 65, 'avg' => 0.278, 'rbi' => 147 }, <<EOY
avg: 0.278
hr: 65
rbi: 147
EOY
		)
	end

	def test_spec_simple_map_with_nested_sequences
		# Simple mapping with nested sequences
		assert_to_yaml(
			{ 'american' =>
			  [ 'Boston Red Sox', 'Detroit Tigers', 'New York Yankees' ],
			  'national' =>
			  [ 'New York Mets', 'Chicago Cubs', 'Atlanta Braves' ] }, <<EOY
american:
  - Boston Red Sox
  - Detroit Tigers
  - New York Yankees
national:
  - New York Mets
  - Chicago Cubs
  - Atlanta Braves
EOY
		)
	end

	def test_spec_simple_sequence_with_nested_map
		# Simple sequence with nested map
		assert_to_yaml(
		  [
		    {'name' => 'Mark McGwire', 'hr' => 65, 'avg' => 0.278},
			{'name' => 'Sammy Sosa', 'hr' => 63, 'avg' => 0.288}
		  ], <<EOY
-
  avg: 0.278
  hr: 65
  name: Mark McGwire
-
  avg: 0.288
  hr: 63
  name: Sammy Sosa
EOY
		)
	end

	def test_spec_sequence_of_sequences
		# Simple sequence with inline sequences
		assert_parse_only(
		  [
		  	[ 'name', 'hr', 'avg' ],
			[ 'Mark McGwire', 65, 0.278 ],
			[ 'Sammy Sosa', 63, 0.288 ]
		  ], <<EOY
- [ name         , hr , avg   ]
- [ Mark McGwire , 65 , 0.278 ]
- [ Sammy Sosa   , 63 , 0.288 ]
EOY
		)
	end

	def test_spec_mapping_of_mappings
		# Simple map with inline maps
		assert_parse_only(
		  { 'Mark McGwire' =>
		    { 'hr' => 65, 'avg' => 0.278 },
			'Sammy Sosa' =>
		    { 'hr' => 63, 'avg' => 0.288 }
		  }, <<EOY
Mark McGwire: {hr: 65, avg: 0.278}
Sammy Sosa:   {hr: 63,
               avg: 0.288}
EOY
		)
	end

    def test_ambiguous_comments
        # [ruby-talk:88012]
        assert_to_yaml( "Call the method #dave", <<EOY )
--- "Call the method #dave"
EOY
    end

	def test_spec_nested_comments
		# Map and sequences with comments
		assert_parse_only(
		  { 'hr' => [ 'Mark McGwire', 'Sammy Sosa' ],
		    'rbi' => [ 'Sammy Sosa', 'Ken Griffey' ] }, <<EOY
hr: # 1998 hr ranking
  - Mark McGwire
  - Sammy Sosa
rbi:
  # 1998 rbi ranking
  - Sammy Sosa
  - Ken Griffey
EOY
		)
	end

	def test_spec_anchors_and_aliases
		# Anchors and aliases
		assert_parse_only(
			{ 'hr' =>
			  [ 'Mark McGwire', 'Sammy Sosa' ],
			  'rbi' =>
			  [ 'Sammy Sosa', 'Ken Griffey' ] }, <<EOY
hr:
   - Mark McGwire
   # Name "Sammy Sosa" scalar SS
   - &SS Sammy Sosa
rbi:
   # So it can be referenced later.
   - *SS
   - Ken Griffey
EOY
	 	)

        assert_to_yaml(
            [{"arrival"=>"EDI", "departure"=>"LAX", "fareref"=>"DOGMA", "currency"=>"GBP"}, {"arrival"=>"MEL", "departure"=>"SYD", "fareref"=>"MADF", "currency"=>"AUD"}, {"arrival"=>"MCO", "departure"=>"JFK", "fareref"=>"DFSF", "currency"=>"USD"}], <<EOY
  -
    &F fareref: DOGMA
    &C currency: GBP
    &D departure: LAX
    &A arrival: EDI
  - { *F: MADF, *C: AUD, *D: SYD, *A: MEL }
  - { *F: DFSF, *C: USD, *D: JFK, *A: MCO }
EOY
        )

        assert_to_yaml(
            {"ALIASES"=>["fareref", "currency", "departure", "arrival"], "FARES"=>[{"arrival"=>"EDI", "departure"=>"LAX", "fareref"=>"DOGMA", "currency"=>"GBP"}, {"arrival"=>"MEL", "departure"=>"SYD", "fareref"=>"MADF", "currency"=>"AUD"}, {"arrival"=>"MCO", "departure"=>"JFK", "fareref"=>"DFSF", "currency"=>"USD"}]}, <<EOY
---
ALIASES: [&f fareref, &c currency, &d departure, &a arrival]
FARES:
- *f: DOGMA
  *c: GBP
  *d: LAX
  *a: EDI

- *f: MADF
  *c: AUD
  *d: SYD
  *a: MEL

- *f: DFSF
  *c: USD
  *d: JFK
  *a: MCO

EOY
        )

	end

	def test_spec_mapping_between_sequences
		# Complex key #1
		dj = Date.new( 2001, 7, 23 )
		assert_parse_only(
			{ [ 'Detroit Tigers', 'Chicago Cubs' ] => [ Date.new( 2001, 7, 23 ) ],
			  [ 'New York Yankees', 'Atlanta Braves' ] => [ Date.new( 2001, 7, 2 ), Date.new( 2001, 8, 12 ), Date.new( 2001, 8, 14 ) ] }, <<EOY
? # PLAY SCHEDULE
  - Detroit Tigers
  - Chicago Cubs
:
  - 2001-07-23

? [ New York Yankees,
    Atlanta Braves ]
: [ 2001-07-02, 2001-08-12,
    2001-08-14 ]
EOY
		)

		# Complex key #2
		assert_parse_only(
		  { [ 'New York Yankees', 'Atlanta Braves' ] =>
		    [ Date.new( 2001, 7, 2 ), Date.new( 2001, 8, 12 ),
			  Date.new( 2001, 8, 14 ) ],
			[ 'Detroit Tigers', 'Chicago Cubs' ] =>
			[ Date.new( 2001, 7, 23 ) ]
		  }, <<EOY
?
    - New York Yankees
    - Atlanta Braves
:
  - 2001-07-02
  - 2001-08-12
  - 2001-08-14
?
    - Detroit Tigers
    - Chicago Cubs
:
  - 2001-07-23
EOY
		)
	end

	def test_spec_sequence_key_shortcut
		# Shortcut sequence map
		assert_parse_only(
		  { 'invoice' => 34843, 'date' => Date.new( 2001, 1, 23 ),
		    'bill-to' => 'Chris Dumars', 'product' =>
			[ { 'item' => 'Super Hoop', 'quantity' => 1 },
			  { 'item' => 'Basketball', 'quantity' => 4 },
			  { 'item' => 'Big Shoes', 'quantity' => 1 } ] }, <<EOY
invoice: 34843
date   : 2001-01-23
bill-to: Chris Dumars
product:
  - item    : Super Hoop
    quantity: 1
  - item    : Basketball
    quantity: 4
  - item    : Big Shoes
    quantity: 1
EOY
		)
	end

    def test_spec_sequence_in_sequence_shortcut
        # Seq-in-seq
        assert_parse_only( [ [ [ 'one', 'two', 'three' ] ] ], <<EOY )
- - - one
    - two
    - three
EOY
    end

    def test_spec_sequence_shortcuts
        # Sequence shortcuts combined
        assert_parse_only(
[
  [
    [ [ 'one' ] ],
    [ 'two', 'three' ],
    { 'four' => nil },
    [ { 'five' => [ 'six' ] } ],
    [ 'seven' ]
  ],
  [ 'eight', 'nine' ]
], <<EOY )
- - - - one
  - - two
    - three
  - four:
  - - five:
      - six
  - - seven
- - eight
  - nine
EOY
    end

	def test_spec_single_literal
		# Literal scalar block
		assert_parse_only( [ "\\/|\\/|\n/ |  |_\n" ], <<EOY )
- |
    \\/|\\/|
    / |  |_
EOY
	end

	def test_spec_single_folded
		# Folded scalar block
		assert_parse_only(
			[ "Mark McGwire's year was crippled by a knee injury.\n" ], <<EOY
- >
    Mark McGwire\'s
    year was crippled
    by a knee injury.
EOY
		)
	end

	def test_spec_preserve_indent
		# Preserve indented spaces
		assert_parse_only(
			"Sammy Sosa completed another fine season with great stats.\n\n  63 Home Runs\n  0.288 Batting Average\n\nWhat a year!\n", <<EOY
--- >
 Sammy Sosa completed another
 fine season with great stats.

   63 Home Runs
   0.288 Batting Average

 What a year!
EOY
		)
	end

	def test_spec_indentation_determines_scope
		assert_parse_only(
			{ 'name' => 'Mark McGwire', 'accomplishment' => "Mark set a major league home run record in 1998.\n",
			  'stats' => "65 Home Runs\n0.278 Batting Average\n" }, <<EOY
name: Mark McGwire
accomplishment: >
   Mark set a major league
   home run record in 1998.
stats: |
   65 Home Runs
   0.278 Batting Average
EOY
		)
	end

	def test_spec_multiline_scalars
		# Multiline flow scalars
	 	assert_parse_only(
	 		{ 'plain' => 'This unquoted scalar spans many lines.',
	 		  'quoted' => "So does this quoted scalar.\n" }, <<EOY
plain: This unquoted
       scalar spans
       many lines.
quoted: "\\
  So does this quoted
  scalar.\\n"
EOY
		)
	end

	def test_spec_type_int
		assert_parse_only(
			{ 'canonical' => 12345, 'decimal' => 12345, 'octal' => '014'.oct, 'hexadecimal' => '0xC'.hex }, <<EOY
canonical: 12345
decimal: +12,345
octal: 014
hexadecimal: 0xC
EOY
		)
		assert_parse_only(
            { 'canonical' => 685230, 'decimal' => 685230, 'octal' => 02472256, 'hexadecimal' => 0x0A74AE, 'sexagesimal' => 685230 }, <<EOY)
canonical: 685230
decimal: +685,230
octal: 02472256
hexadecimal: 0x0A,74,AE
sexagesimal: 190:20:30
EOY
	end

	def test_spec_type_float
		assert_parse_only(
			{ 'canonical' => 1230.15, 'exponential' => 1230.15, 'fixed' => 1230.15,
			  'negative infinity' => -1.0/0.0 }, <<EOY)
canonical: 1.23015e+3
exponential: 12.3015e+02
fixed: 1,230.15
negative infinity: -.inf
EOY
		nan = Psych::load( <<EOY )
not a number: .NaN
EOY
		assert( nan['not a number'].nan? )
	end

	def test_spec_type_misc
		assert_parse_only(
			{ nil => nil, true => true, false => false, 'string' => '12345' }, <<EOY
null: ~
true: yes
false: no
string: '12345'
EOY
		)
	end

	def test_spec_complex_invoice
		# Complex invoice type
		id001 = { 'given' => 'Chris', 'family' => 'Dumars', 'address' =>
			{ 'lines' => "458 Walkman Dr.\nSuite #292\n", 'city' => 'Royal Oak',
			  'state' => 'MI', 'postal' => 48046 } }
		assert_parse_only(
			{ 'invoice' => 34843, 'date' => Date.new( 2001, 1, 23 ),
			  'bill-to' => id001, 'ship-to' => id001, 'product' =>
			  [ { 'sku' => 'BL394D', 'quantity' => 4,
			      'description' => 'Basketball', 'price' => 450.00 },
				{ 'sku' => 'BL4438H', 'quantity' => 1,
				  'description' => 'Super Hoop', 'price' => 2392.00 } ],
			  'tax' => 251.42, 'total' => 4443.52,
			  'comments' => "Late afternoon is best. Backup contact is Nancy Billsmer @ 338-4338.\n" }, <<EOY
invoice: 34843
date   : 2001-01-23
bill-to: &id001
    given  : Chris
    family : !str Dumars
    address:
        lines: |
            458 Walkman Dr.
            Suite #292
        city    : Royal Oak
        state   : MI
        postal  : 48046
ship-to: *id001
product:
    - !map
      sku         : BL394D
      quantity    : 4
      description : Basketball
      price       : 450.00
    - sku         : BL4438H
      quantity    : 1
      description : Super Hoop
      price       : 2392.00
tax  : 251.42
total: 4443.52
comments: >
    Late afternoon is best.
    Backup contact is Nancy
    Billsmer @ 338-4338.
EOY
		)
	end

	def test_spec_log_file
		doc_ct = 0
		Psych::load_documents( <<EOY
---
Time: 2001-11-23 15:01:42 -05:00
User: ed
Warning: >
  This is an error message
  for the log file
---
Time: 2001-11-23 15:02:31 -05:00
User: ed
Warning: >
  A slightly different error
  message.
---
Date: 2001-11-23 15:03:17 -05:00
User: ed
Fatal: >
  Unknown variable "bar"
Stack:
  - file: TopClass.py
    line: 23
    code: |
      x = MoreObject("345\\n")
  - file: MoreClass.py
    line: 58
    code: |-
      foo = bar
EOY
		) { |doc|
			case doc_ct
				when 0
					assert_equal( doc, { 'Time' => mktime( 2001, 11, 23, 15, 01, 42, 00, "-05:00" ),
						'User' => 'ed', 'Warning' => "This is an error message for the log file\n" } )
				when 1
					assert_equal( doc, { 'Time' => mktime( 2001, 11, 23, 15, 02, 31, 00, "-05:00" ),
						'User' => 'ed', 'Warning' => "A slightly different error message.\n" } )
				when 2
					assert_equal( doc, { 'Date' => mktime( 2001, 11, 23, 15, 03, 17, 00, "-05:00" ),
						'User' => 'ed', 'Fatal' => "Unknown variable \"bar\"\n",
						'Stack' => [
							{ 'file' => 'TopClass.py', 'line' => 23, 'code' => "x = MoreObject(\"345\\n\")\n" },
							{ 'file' => 'MoreClass.py', 'line' => 58, 'code' => "foo = bar" } ] } )
			end
			doc_ct += 1
		}
		assert_equal( doc_ct, 3 )
	end

	def test_spec_root_fold
		y = Psych::load( <<EOY
---
This Psych stream contains a single text value.
The next stream is a log file - a sequence of
log entries. Adding an entry to the log is a
simple matter of appending it at the end.
EOY
		)
		assert_equal( y, "This Psych stream contains a single text value. The next stream is a log file - a sequence of log entries. Adding an entry to the log is a simple matter of appending it at the end." )
	end

	def test_spec_root_mapping
		y = Psych::load( <<EOY
# This stream is an example of a top-level mapping.
invoice : 34843
date    : 2001-01-23
total   : 4443.52
EOY
		)
		assert_equal( y, { 'invoice' => 34843, 'date' => Date.new( 2001, 1, 23 ), 'total' => 4443.52 } )
	end

	def test_spec_oneline_docs
		doc_ct = 0
		Psych::load_documents( <<EOY
# The following is a sequence of three documents.
# The first contains an empty mapping, the second
# an empty sequence, and the last an empty string.
--- {}
--- [ ]
--- ''
EOY
		) { |doc|
			case doc_ct
				when 0
					assert_equal( doc, {} )
				when 1
					assert_equal( doc, [] )
				when 2
					assert_equal( doc, '' )
			end
			doc_ct += 1
		}
		assert_equal( doc_ct, 3 )
	end

	def test_spec_domain_prefix
        customer_proc = proc { |type, val|
            if Hash === val
                scheme, domain, type = type.split( ':', 3 )
                val['type'] = "domain #{type}"
                val
            else
                raise ArgumentError, "Not a Hash in domain.tld,2002/invoice: " + val.inspect
            end
        }
        Psych.add_domain_type( "domain.tld,2002", 'invoice', &customer_proc )
        Psych.add_domain_type( "domain.tld,2002", 'customer', &customer_proc )
		assert_parse_only( { "invoice"=> { "customers"=> [ { "given"=>"Chris", "type"=>"domain customer", "family"=>"Dumars" } ], "type"=>"domain invoice" } }, <<EOY
# 'http://domain.tld,2002/invoice' is some type family.
invoice: !domain.tld,2002/invoice
  # 'seq' is shorthand for 'http://yaml.org/seq'.
  # This does not effect '^customer' below
  # because it is does not specify a prefix.
  customers: !seq
    # '^customer' is shorthand for the full
    # notation 'http://domain.tld,2002/customer'.
    - !customer
      given : Chris
      family : Dumars
EOY
		)
	end

	def test_spec_throwaway
		assert_parse_only(
			{"this"=>"contains three lines of text.\nThe third one starts with a\n# character. This isn't a comment.\n"}, <<EOY
### These are four throwaway comment  ###

### lines (the second line is empty). ###
this: |   # Comments may trail lines.
    contains three lines of text.
    The third one starts with a
    # character. This isn't a comment.

# These are three throwaway comment
# lines (the first line is empty).
EOY
		)
	end

	def test_spec_force_implicit
		# Force implicit
		assert_parse_only(
			{ 'integer' => 12, 'also int' => 12, 'string' => '12' }, <<EOY
integer: 12
also int: ! "12"
string: !str 12
EOY
		)
	end

    ###
    # Commenting out this test.  This line:
    #
    #   - !domain.tld,2002/type\\x30 value
    #
    # Is invalid according to the YAML spec:
    #
    #   http://yaml.org/spec/1.1/#id896876
    #
#	def test_spec_url_escaping
#		Psych.add_domain_type( "domain.tld,2002", "type0" ) { |type, val|
#			"ONE: #{val}"
#		}
#		Psych.add_domain_type( "domain.tld,2002", "type%30" ) { |type, val|
#			"TWO: #{val}"
#		}
#		assert_parse_only(
#			{ 'same' => [ 'ONE: value', 'ONE: value' ], 'different' => [ 'TWO: value' ] }, <<EOY
#same:
#  - !domain.tld,2002/type\\x30 value
#  - !domain.tld,2002/type0 value
#different: # As far as the Psych parser is concerned
#  - !domain.tld,2002/type%30 value
#EOY
#		)
#	end

	def test_spec_override_anchor
		# Override anchor
		a001 = "The alias node below is a repeated use of this value.\n"
		assert_parse_only(
			{ 'anchor' => 'This scalar has an anchor.', 'override' => a001, 'alias' => a001 }, <<EOY
anchor : &A001 This scalar has an anchor.
override : &A001 >
 The alias node below is a
 repeated use of this value.
alias : *A001
EOY
		)
	end

	def test_spec_explicit_families
        Psych.add_domain_type( "somewhere.com,2002", 'type' ) { |type, val|
            "SOMEWHERE: #{val}"
        }
		assert_parse_only(
			{ 'not-date' => '2002-04-28', 'picture' => "GIF89a\f\000\f\000\204\000\000\377\377\367\365\365\356\351\351\345fff\000\000\000\347\347\347^^^\363\363\355\216\216\216\340\340\340\237\237\237\223\223\223\247\247\247\236\236\236i^\020' \202\n\001\000;", 'hmm' => "SOMEWHERE: family above is short for\nhttp://somewhere.com/type\n" }, <<EOY
not-date: !str 2002-04-28
picture: !binary |
 R0lGODlhDAAMAIQAAP//9/X
 17unp5WZmZgAAAOfn515eXv
 Pz7Y6OjuDg4J+fn5OTk6enp
 56enmleECcgggoBADs=

hmm: !somewhere.com,2002/type |
 family above is short for
 http://somewhere.com/type
EOY
		)
	end

	def test_spec_application_family
		# Testing the clarkevans.com graphs
		Psych.add_domain_type( "clarkevans.com,2002", 'graph/shape' ) { |type, val|
			if Array === val
				val << "Shape Container"
				val
			else
				raise ArgumentError, "Invalid graph of type #{val.class}: " + val.inspect
			end
		}
		one_shape_proc = Proc.new { |type, val|
			if Hash === val
                type = type.split( /:/ )
				val['TYPE'] = "Shape: #{type[2]}"
				val
			else
				raise ArgumentError, "Invalid graph of type #{val.class}: " + val.inspect
			end
		}
		Psych.add_domain_type( "clarkevans.com,2002", 'graph/circle', &one_shape_proc )
		Psych.add_domain_type( "clarkevans.com,2002", 'graph/line', &one_shape_proc )
		Psych.add_domain_type( "clarkevans.com,2002", 'graph/text', &one_shape_proc )
        # MODIFIED to remove invalid Psych
		assert_parse_only(
			[[{"radius"=>7, "center"=>{"x"=>73, "y"=>129}, "TYPE"=>"Shape: graph/circle"}, {"finish"=>{"x"=>89, "y"=>102}, "TYPE"=>"Shape: graph/line", "start"=>{"x"=>73, "y"=>129}}, {"TYPE"=>"Shape: graph/text", "value"=>"Pretty vector drawing.", "start"=>{"x"=>73, "y"=>129}, "color"=>16772795}, "Shape Container"]], <<EOY
- !clarkevans.com,2002/graph/shape
  - !/graph/circle
    center: &ORIGIN {x: 73, y: 129}
    radius: 7
  - !/graph/line # !clarkevans.com,2002/graph/line
    start: *ORIGIN
    finish: { x: 89, y: 102 }
  - !/graph/text
    start: *ORIGIN
    color: 0xFFEEBB
    value: Pretty vector drawing.
EOY
		)
	end

	def test_spec_float_explicit
		assert_parse_only(
			[ 10.0, 10.0, 10.0, 10.0 ], <<EOY
# All entries in the sequence
# have the same type and value.
- 10.0
- !float 10
- !yaml.org,2002/float '10'
- !yaml.org,2002/float "\\
  1\\
  0"
EOY
		)
	end

	def test_spec_builtin_seq
		# Assortment of sequences
		assert_parse_only(
			{ 'empty' => [], 'in-line' => [ 'one', 'two', 'three', 'four', 'five' ],
			  'nested' => [ 'First item in top sequence', [ 'Subordinate sequence entry' ],
			  	"A multi-line sequence entry\n", 'Sixth item in top sequence' ] }, <<EOY
empty: []
in-line: [ one, two, three # May span lines,
         , four,           # indentation is
           five ]          # mostly ignored.
nested:
 - First item in top sequence
 -
  - Subordinate sequence entry
 - >
   A multi-line
   sequence entry
 - Sixth item in top sequence
EOY
		)
	end

	def test_spec_builtin_map
		# Assortment of mappings
		assert_parse_only(
			{ 'empty' => {}, 'in-line' => { 'one' => 1, 'two' => 2 },
			  'spanning' => { 'one' => 1, 'two' => 2 },
			  'nested' => { 'first' => 'First entry', 'second' =>
			  	{ 'key' => 'Subordinate mapping' }, 'third' =>
				  [ 'Subordinate sequence', {}, 'Previous mapping is empty.',
					{ 'A key' => 'value pair in a sequence.', 'A second' => 'key:value pair.' },
					'The previous entry is equal to the following one.',
					{ 'A key' => 'value pair in a sequence.', 'A second' => 'key:value pair.' } ],
				  12.0 => 'This key is a float.', "?\n" => 'This key had to be protected.',
				  "\a" => 'This key had to be escaped.',
				  "This is a multi-line folded key\n" => "Whose value is also multi-line.\n",
				  [ 'This key', 'is a sequence' ] => [ 'With a sequence value.' ] } }, <<EOY

empty: {}
in-line: { one: 1, two: 2 }
spanning: { one: 1,
   two: 2 }
nested:
 first : First entry
 second:
  key: Subordinate mapping
 third:
  - Subordinate sequence
  - { }
  - Previous mapping is empty.
  - A key: value pair in a sequence.
    A second: key:value pair.
  - The previous entry is equal to the following one.
  -
    A key: value pair in a sequence.
    A second: key:value pair.
 !float 12 : This key is a float.
 ? >
  ?
 : This key had to be protected.
 "\\a" : This key had to be escaped.
 ? >
   This is a
   multi-line
   folded key
 : >
   Whose value is
   also multi-line.
 ?
  - This key
  - is a sequence
 :
  - With a sequence value.
# The following parses correctly,
# but Ruby 1.6.* fails the comparison!
# ?
#  This: key
#  is a: mapping
# :
#  with a: mapping value.
EOY
		)
	end

	def test_spec_builtin_literal_blocks
		# Assortment of literal scalar blocks
		assert_parse_only(
			{"both are equal to"=>"  This has no newline.", "is equal to"=>"The \\ ' \" characters may be\nfreely used. Leading white\n   space is significant.\n\nLine breaks are significant.\nThus this value contains one\nempty line and ends with a\nsingle line break, but does\nnot start with one.\n", "also written as"=>"  This has no newline.", "indented and chomped"=>"  This has no newline.", "empty"=>"", "literal"=>"The \\ ' \" characters may be\nfreely used. Leading white\n   space is significant.\n\nLine breaks are significant.\nThus this value contains one\nempty line and ends with a\nsingle line break, but does\nnot start with one.\n"}, <<EOY
empty: |

literal: |
 The \\\ ' " characters may be
 freely used. Leading white
    space is significant.

 Line breaks are significant.
 Thus this value contains one
 empty line and ends with a
 single line break, but does
 not start with one.

is equal to: "The \\\\ ' \\" characters may \\
 be\\nfreely used. Leading white\\n   space \\
 is significant.\\n\\nLine breaks are \\
 significant.\\nThus this value contains \\
 one\\nempty line and ends with a\\nsingle \\
 line break, but does\\nnot start with one.\\n"

# Comments may follow a nested
# scalar value. They must be
# less indented.

# Modifiers may be combined in any order.
indented and chomped: |2-
    This has no newline.

also written as: |-2
    This has no newline.

both are equal to: "  This has no newline."
EOY
		)

		str1 = "This has one newline.\n"
		str2 = "This has no newline."
		str3 = "This has two newlines.\n\n"
		assert_parse_only(
			{ 'clipped' => str1, 'same as "clipped" above' => str1,
			  'stripped' => str2, 'same as "stripped" above' => str2,
			  'kept' => str3, 'same as "kept" above' => str3 }, <<EOY
clipped: |
    This has one newline.

same as "clipped" above: "This has one newline.\\n"

stripped: |-
    This has no newline.

same as "stripped" above: "This has no newline."

kept: |+
    This has two newlines.

same as "kept" above: "This has two newlines.\\n\\n"

EOY
		)
	end

	def test_spec_span_single_quote
		assert_parse_only( {"third"=>"a single quote ' must be escaped.", "second"=>"! : \\ etc. can be used freely.", "is same as"=>"this contains six spaces\nand one line break", "empty"=>"", "span"=>"this contains six spaces\nand one line break"}, <<EOY
empty: ''
second: '! : \\ etc. can be used freely.'
third: 'a single quote '' must be escaped.'
span: 'this contains
      six spaces

      and one
      line break'
is same as: "this contains six spaces\\nand one line break"
EOY
		)
	end

	def test_spec_span_double_quote
		assert_parse_only( {"is equal to"=>"this contains four  spaces", "third"=>"a \" or a \\ must be escaped.", "second"=>"! : etc. can be used freely.", "empty"=>"", "fourth"=>"this value ends with an LF.\n", "span"=>"this contains four  spaces"}, <<EOY
empty: ""
second: "! : etc. can be used freely."
third: "a \\\" or a \\\\ must be escaped."
fourth: "this value ends with an LF.\\n"
span: "this contains
  four  \\
      spaces"
is equal to: "this contains four  spaces"
EOY
		)
	end

	def test_spec_builtin_time
		# Time
		assert_parse_only(
			{ "space separated" => mktime( 2001, 12, 14, 21, 59, 43, ".10", "-05:00" ),
			  "canonical" => mktime( 2001, 12, 15, 2, 59, 43, ".10" ),
			  "date (noon UTC)" => Date.new( 2002, 12, 14),
			  "valid iso8601" => mktime( 2001, 12, 14, 21, 59, 43, ".10", "-05:00" ) }, <<EOY
canonical: 2001-12-15T02:59:43.1Z
valid iso8601: 2001-12-14t21:59:43.10-05:00
space separated: 2001-12-14 21:59:43.10 -05:00
date (noon UTC): 2002-12-14
EOY
		)
	end

	def test_spec_builtin_binary
		arrow_gif = "GIF89a\f\000\f\000\204\000\000\377\377\367\365\365\356\351\351\345fff\000\000\000\347\347\347^^^\363\363\355\216\216\216\340\340\340\237\237\237\223\223\223\247\247\247\236\236\236iiiccc\243\243\243\204\204\204\377\376\371\377\376\371\377\376\371\377\376\371\377\376\371\377\376\371\377\376\371\377\376\371\377\376\371\377\376\371\377\376\371\377\376\371\377\376\371\377\376\371!\376\016Made with GIMP\000,\000\000\000\000\f\000\f\000\000\005,  \216\2010\236\343@\024\350i\020\304\321\212\010\034\317\200M$z\357\3770\205p\270\2601f\r\e\316\001\303\001\036\020' \202\n\001\000;"
		assert_parse_only(
			{ 'canonical' => arrow_gif, 'base64' => arrow_gif,
			  'description' => "The binary value above is a tiny arrow encoded as a gif image.\n" }, <<EOY
canonical: !binary "\\
 R0lGODlhDAAMAIQAAP//9/X17unp5WZmZgAAAOf\\
 n515eXvPz7Y6OjuDg4J+fn5OTk6enp56enmlpaW\\
 NjY6Ojo4SEhP/++f/++f/++f/++f/++f/++f/++\\
 f/++f/++f/++f/++f/++f/++f/++SH+Dk1hZGUg\\
 d2l0aCBHSU1QACwAAAAADAAMAAAFLCAgjoEwnuN\\
 AFOhpEMTRiggcz4BNJHrv/zCFcLiwMWYNG84Bww\\
 EeECcgggoBADs="
base64: !binary |
 R0lGODlhDAAMAIQAAP//9/X17unp5WZmZgAAAOf
 n515eXvPz7Y6OjuDg4J+fn5OTk6enp56enmlpaW
 NjY6Ojo4SEhP/++f/++f/++f/++f/++f/++f/++
 f/++f/++f/++f/++f/++f/++f/++SH+Dk1hZGUg
 d2l0aCBHSU1QACwAAAAADAAMAAAFLCAgjoEwnuN
 AFOhpEMTRiggcz4BNJHrv/zCFcLiwMWYNG84Bww
 EeECcgggoBADs=
description: >
 The binary value above is a tiny arrow
 encoded as a gif image.
EOY
		)
	end
	def test_ruby_regexp
		# Test Ruby regular expressions
		assert_to_yaml(
			{ 'simple' => /a.b/, 'complex' => %r'\A"((?:[^"]|\")+)"',
			  'case-insensitive' => /George McFly/i }, <<EOY
case-insensitive: !ruby/regexp "/George McFly/i"
complex: !ruby/regexp "/\\\\A\\"((?:[^\\"]|\\\\\\")+)\\"/"
simple: !ruby/regexp "/a.b/"
EOY
		)
	end

    #
    # Test of Ranges
    #
    def test_ranges

        # Simple numeric
        assert_to_yaml( 1..3, <<EOY )
--- !ruby/range 1..3
EOY

        # Simple alphabetic
        assert_to_yaml( 'a'..'z', <<EOY )
--- !ruby/range a..z
EOY

        # Float
        assert_to_yaml( 10.5...30.3, <<EOY )
--- !ruby/range 10.5...30.3
EOY

    end

	def test_ruby_struct
		# Ruby structures
		book_struct = Struct::new( "MyBookStruct", :author, :title, :year, :isbn )
		assert_to_yaml(
			[ book_struct.new( "Yukihiro Matsumoto", "Ruby in a Nutshell", 2002, "0-596-00214-9" ),
			  book_struct.new( [ 'Dave Thomas', 'Andy Hunt' ], "The Pickaxe", 2002,
				book_struct.new( "This should be the ISBN", "but I have another struct here", 2002, "None" )
			  ) ], <<EOY
- !ruby/struct:MyBookStruct
  author: Yukihiro Matsumoto
  title: Ruby in a Nutshell
  year: 2002
  isbn: 0-596-00214-9
- !ruby/struct:MyBookStruct
  author:
    - Dave Thomas
    - Andy Hunt
  title: The Pickaxe
  year: 2002
  isbn: !ruby/struct:MyBookStruct
    author: This should be the ISBN
    title: but I have another struct here
    year: 2002
    isbn: None
EOY
		)

        assert_to_yaml( Psych_Tests::StructTest.new( 123 ), <<EOY )
--- !ruby/struct:Psych_Tests::StructTest
c: 123
EOY

	end

	def test_ruby_rational
		assert_to_yaml( Rational(1, 2), <<EOY )
--- !ruby/object:Rational
numerator: 1
denominator: 2
EOY

		# Read Psych dumped by the ruby 1.8.3.
		assert_to_yaml( Rational(1, 2), "!ruby/object:Rational 1/2\n" )
		assert_raises( ArgumentError ) { Psych.load("!ruby/object:Rational INVALID/RATIONAL\n") }
	end

	def test_ruby_complex
		assert_to_yaml( Complex(3, 4), <<EOY )
--- !ruby/object:Complex
image: 4
real: 3
EOY

		# Read Psych dumped by the ruby 1.8.3.
		assert_to_yaml( Complex(3, 4), "!ruby/object:Complex 3+4i\n" )
		assert_raises( ArgumentError ) { Psych.load("!ruby/object:Complex INVALID+COMPLEXi\n") }
	end

	def test_emitting_indicators
		assert_to_yaml( "Hi, from Object 1. You passed: please, pretty please", <<EOY
--- "Hi, from Object 1. You passed: please, pretty please"
EOY
		)
	end

	##
	## Test the Psych::Stream class -- INACTIVE at the moment
	##
	#def test_document
	#	y = Psych::Stream.new( :Indent => 2, :UseVersion => 0 )
	#	y.add(
	#		{ 'hi' => 'hello', 'map' =>
	#			{ 'good' => 'two' },
	#		  'time' => Time.now,
	#		  'try' => /^po(.*)$/,
	#		  'bye' => 'goodbye'
	#		}
	#	)
	#	y.add( { 'po' => 'nil', 'oper' => 90 } )
	#	y.add( { 'hi' => 'wow!', 'bye' => 'wow!' } )
	#	y.add( { [ 'Red Socks', 'Boston' ] => [ 'One', 'Two', 'Three' ] } )
	#	y.add( [ true, false, false ] )
	#end

    #
    # Test YPath choices parsing
    #
    #def test_ypath_parsing
    #    assert_path_segments( "/*/((one|three)/name|place)|//place",
    #      [ ["*", "one", "name"],
    #        ["*", "three", "name"],
    #        ["*", "place"],
    #        ["/", "place"] ]
    #    )
    #end

    #
    # Tests from Tanaka Akira on [ruby-core]
    #
    def test_akira

        # Commas in plain scalars [ruby-core:1066]
        assert_to_yaml(
            {"A"=>"A,","B"=>"B"}, <<EOY
A: "A,"
B: B
EOY
        )

        # Double-quoted keys [ruby-core:1069]
        assert_to_yaml(
            {"1"=>2, "2"=>3}, <<EOY
'1': 2
"2": 3
EOY
        )

        # Anchored mapping [ruby-core:1071]
        assert_to_yaml(
            [{"a"=>"b"}] * 2, <<EOY
- &id001
  a: b
- *id001
EOY
        )

        # Stress test [ruby-core:1071]
        # a = []; 1000.times { a << {"a"=>"b", "c"=>"d"} }
        # Psych::load( a.to_yaml )

    end

    #
    # Test Time.now cycle
    #
    def test_time_now_cycle
        #
        # From Minero Aoki [ruby-core:2305]
        #
        #require 'yaml'
        t = Time.now
        t = Time.at(t.tv_sec, t.tv_usec)
        5.times do
            assert_cycle(t)
        end
    end

    #
    # Test Range cycle
    #
    def test_range_cycle
      #
      # From Minero Aoki [ruby-core:02306]
      #
      assert_cycle("a".."z")

      #
      # From Nobu Nakada [ruby-core:02311]
      #
      assert_cycle(0..1)
      assert_cycle(1.0e20 .. 2.0e20)
      assert_cycle("0".."1")
      assert_cycle(".."..."...")
      assert_cycle(".rb"..".pl")
      assert_cycle(".rb"...".pl")
      assert_cycle('"'...".")
      assert_cycle("'"...".")
    end

    #
    # Circular references
    #
    def test_circular_references
        a = []; a[0] = a; a[1] = a
        inspect_str = "[[...], [...]]"
        assert_equal( inspect_str, Psych::load(Psych.dump(a)).inspect )
    end

    #
    # Test Symbol cycle
    #
    def test_symbol_cycle
      #
      # From Aaron Schrab [ruby-Bugs:2535]
      #
      assert_cycle(:"^foo")
    end

    #
    # Test Numeric cycle
    #
    class NumericTest < Numeric
      def initialize(value)
        @value = value
      end
      def ==(other)
        @value == other.instance_eval{ @value }
      end
    end
    def test_numeric_cycle
      assert_cycle(1) # Fixnum
      assert_cycle(111111111111111111111111111111111) # Bignum
      assert_cycle(NumericTest.new(3)) # Subclass of Numeric
    end

    #
    # Test empty map/seq in map cycle
    #
    def test_empty_map_key
      #
      # empty seq as key
      #
      assert_cycle({[]=>""})

      #
      # empty map as key
      #
      assert_cycle({{}=>""})
    end

    #
    # contributed by riley lynch [ruby-Bugs-8548]
    #
    def test_object_id_collision
      omap = Psych::Omap.new
      1000.times { |i| omap["key_#{i}"] = { "value" => i } }
      raise "id collision in ordered map" if Psych.dump(omap) =~ /id\d+/
    end

    def test_date_out_of_range
      Psych::load('1900-01-01T00:00:00+00:00')
    end

    def test_normal_exit
      Psych.load("2000-01-01 00:00:00.#{"0"*1000} +00:00\n")
      # '[ruby-core:13735]'
    end
end

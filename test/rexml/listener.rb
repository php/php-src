class Listener
	attr_reader :ts, :te
	attr_reader :normalize
	def initialize
		@ts = false
		@te = false
	end
	def tag_start name, attrs
		@ts = true if name=="subsection" and attrs["title"]=="Namespaces"
	end
	def tag_end name
		@te = true if name=="documentation"
	end
	def text text
		@normalize = text
		#text.tr! "\n", ' '
		#puts "text #{text[0..10]}..."
	end
	def instruction name, instruction
		#puts "instruction"
	end
	def comment comment
		#puts "comment #{comment[0..10]}..."
	end
	def doctype name, pub_sys, long_name, uri
		#puts "doctype #{name}"
	end
	def attlistdecl content
		#puts "attlistdecl"
	end
	def elementdecl content
		#puts "elementdecl"
	end
	def entitydecl content
		#puts "entitydecl"
	end
	def notationdecl content
		#puts "notationdecl"
	end
	def entity content
		#puts "entity"
	end
	def cdata content
		#puts "cdata"
	end
	def xmldecl version, encoding, standalone
		#puts "xmldecl #{version}"
	end
end


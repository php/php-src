 -- forwardable.rb
						
                                                $Release Version: 1.1 $
                                                $Revision$
						Original version by Tosh

=begin

= Forwardable

A Module to define delegations for selected methods to a class.

== Usage

Using through extending the class.
  
  class Foo
    extend Forwardable

    def_delegators("@out", "printf", "print")
    def_delegators(:@in, :gets)
    def_delegator(:@contents, :[], "content_at")
  end
  f = Foo.new
  f.printf ...
  f.gets
  f.content_at(1)

== Methods

--- Forwardable#def_instance_delegators(accessor, *methods)

      adding the delegations for each method of ((|methods|)) to
      ((|accessor|)).

--- Forwardable#def_instance_delegator(accessor, method, ali = method)
      
      adding the delegation for ((|method|)) to ((|accessor|)). When
      you give optional argument ((|ali|)), ((|ali|)) is used as the
      name of the delegation method, instead of ((|method|)).

--- Forwardable#def_delegators(accessor, *methods)

      the alias of ((|Forwardable#def_instance_delegators|)).

--- Forwardable#def_delegator(accessor, method, ali = method)
      
      the alias of ((|Forwardable#def_instance_delegator|)).

= SingleForwardable

a Module to define delegations for selected methods to an object.

== Usage

Using through extending the object.

  g = Goo.new
  g.extend SingleForwardable
  g.def_delegator("@out", :puts)
  g.puts ...

== Methods

--- SingleForwardable#def_singleton_delegators(accessor, *methods)

      adding the delegations for each method of ((|methods|)) to
      ((|accessor|)).

--- SingleForwardable#def_singleton_delegator(accessor, method, ali = method)

      adding the delegation for ((|method|)) to ((|accessor|)). When
      you give optional argument ((|ali|)), ((|ali|)) is used as the
      name of the delegation method, instead of ((|method|)).

--- SingleForwardable#def_delegators(accessor, *methods)

      the alias of ((|SingleForwardable#def_instance_delegators|)).

--- SingleForwardable#def_delegator(accessor, method, ali = method)

      the alias of ((|SingleForwardable#def_instance_delegator|)).
=end

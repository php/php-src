assert_normal_exit %q{
a = []
ms = "a".."k"
("A".."Z").each do |mod|
  mod = eval("module #{mod}; self; end")
  ms.each do |meth|
    iseq = RubyVM::InstructionSequence.compile("module #{mod}; def #{meth}; end; end")
    GC.stress = true
    iseq.eval
    GC.stress = false
  end
  o = Object.new.extend(mod)
  ms.each do |meth|
    o.send(meth)
  end
end
}, '[ruby-dev:39453]'

assert_normal_exit %q{
a = []
ms = "a".."k"
("A".."Z").each do |mod|
  mod = eval("module #{mod}; self; end")
  ms.each do |meth|
    GC.stress = true
    mod.module_eval {define_method(meth) {}}
    GC.stress = false
  end
  o = Object.new.extend(mod)
  ms.each do |meth|
    o.send(meth)
  end
end
}, '[ruby-dev:39453]'

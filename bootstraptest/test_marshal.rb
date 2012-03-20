
assert_normal_exit %q{
  Marshal.load(Marshal.dump({"k"=>"v"}), lambda {|v| v})
}


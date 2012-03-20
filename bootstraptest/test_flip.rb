assert_equal %q{E}, %q{$_ = "E"; eval("nil if true..~/^E/",nil,"-e"); $_}

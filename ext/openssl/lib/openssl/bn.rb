#--
#
# $RCSfile$
#
# = Ruby-space definitions that completes C-space funcs for BN
# 
# = Info
# 'OpenSSL for Ruby 2' project
# Copyright (C) 2002  Michal Rokos <m.rokos@sh.cvut.cz>
# All rights reserved.
# 
# = Licence
# This program is licenced under the same licence as Ruby.
# (See the file 'LICENCE'.)
# 
# = Version
# $Id$
#
#++

module OpenSSL
  class BN
    include Comparable
  end # BN
end # OpenSSL

##
# Add double dispatch to Integer
#
class Integer
  def to_bn
    OpenSSL::BN::new(self.to_s(16), 16)
  end
end # Integer


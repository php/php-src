#--
# Copyright 2006 by Chad Fowler, Rich Kilmer, Jim Weirich and others.
# All rights reserved.
# See LICENSE.txt for permissions.
#++

# N.B. This file is used by Config.datadir in rubygems.rb, and must not be
# removed before that require is removed. I require to avoid warning more than
# once.

warn 'rbconfig/datadir.rb and {Rb}Config.datadir is being deprecated from '\
 'RubyGems. It will be removed completely on or after June 2011. If you '\
 'wish to rely on a datadir, please use Gem.datadir.'

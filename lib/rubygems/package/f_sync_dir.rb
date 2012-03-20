# -*- coding: utf-8 -*-
#--
# Copyright (C) 2004 Mauricio Julio Fernández Pradier
# See LICENSE.txt for additional licensing information.
#++

module Gem::Package::FSyncDir

  private

  ##
  # make sure this hits the disc

  def fsync_dir(dirname)
    dir = open dirname, 'r'
    dir.fsync
  rescue # ignore IOError if it's an unpatched (old) Ruby
  ensure
    dir.close if dir rescue nil
  end

end


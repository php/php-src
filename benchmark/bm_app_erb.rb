#
# Create many HTML strings with ERB.
#

require 'erb'

data = DATA.read
max = 15_000
title = "hello world!"
content = "hello world!\n" * 10

max.times{
  ERB.new(data).result(binding)
}

__END__

<html>
  <head> <%= title %> </head>
  <body>
    <h1> <%= title %> </h1>
    <p>
      <%= content %>
    </p>
  </body>
</html>

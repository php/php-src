#!/usr/local/bin/ruby

#
# linear.rb
#
# Solves linear equation system(A*x = b) by LU decomposition method.
#  where  A is a coefficient matrix,x is an answer vector,b is a constant vector.
#
# USAGE:
#   ruby linear.rb [input file solved]
#

require "bigdecimal"
require "bigdecimal/ludcmp"

#
# NOTE:
#   Change following BigDecimal::limit() if needed.
BigDecimal::limit(100)
#

include LUSolve
def rd_order(na)
   printf("Number of equations ?") if(na <= 0)
   n = ARGF.gets().to_i
end

na   = ARGV.size
zero = BigDecimal::new("0.0")
one  = BigDecimal::new("1.0")

while (n=rd_order(na))>0
  a = []
  as= []
  b = []
  if na <= 0
     # Read data from console.
     printf("\nEnter coefficient matrix element A[i,j]\n");
     for i in 0...n do
       for j in 0...n do
         printf("A[%d,%d]? ",i,j); s = ARGF.gets
         a  << BigDecimal::new(s);
         as << BigDecimal::new(s);
       end
       printf("Contatant vector element b[%d] ? ",i); b << BigDecimal::new(ARGF.gets);
     end
  else
     # Read data from specified file.
     printf("Coefficient matrix and constant vector.\n");
     for i in 0...n do
       s = ARGF.gets
       printf("%d) %s",i,s)
       s = s.split
       for j in 0...n do
         a  << BigDecimal::new(s[j]);
         as << BigDecimal::new(s[j]);
       end
       b << BigDecimal::new(s[n]);
     end
  end
  x = lusolve(a,b,ludecomp(a,n,zero,one),zero)
  printf("Answer(x[i] & (A*x-b)[i]) follows\n")
  for i in 0...n do
     printf("x[%d]=%s ",i,x[i].to_s)
     s = zero
     for j in 0...n do
       s = s + as[i*n+j]*x[j]
     end
     printf(" & %s\n",(s-b[i]).to_s)
  end
end

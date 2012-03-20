# The Computer Language Shootout
# http://shootout.alioth.debian.org/
# Contributed by Sokolov Yura

def eval_A(i,j)
	return 1.0/((i+j)*(i+j+1)/2+i+1)
end

def eval_A_times_u(u)
        v, i = nil, nil
	(0..u.length-1).collect { |i|
                v = 0
		for j in 0..u.length-1
			v += eval_A(i,j)*u[j]
                end
                v
        }
end

def eval_At_times_u(u)
	v, i = nil, nil
	(0..u.length-1).collect{|i|
                v = 0
		for j in 0..u.length-1
			v += eval_A(j,i)*u[j]
                end
                v
        }
end

def eval_AtA_times_u(u)
	return eval_At_times_u(eval_A_times_u(u))
end

n = 500 # ARGV[0].to_i

u=[1]*n
for i in 1..10
        v=eval_AtA_times_u(u)
        u=eval_AtA_times_u(v)
end
vBv=0
vv=0
for i in 0..n-1
        vBv += u[i]*v[i]
        vv += v[i]*v[i]
end

str = "%0.9f" % (Math.sqrt(vBv/vv)), "\n"
# print str

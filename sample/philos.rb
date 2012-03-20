#
# The Dining Philosophers - thread example
#
require "thread"

srand
#srand
N=9				# number of philosophers
$forks = []
for i in 0..N-1
  $forks[i] = Mutex.new
end
$state = "-o"*N

def wait
  sleep rand(20)/10.0
end

def think(n)
  wait
end

def eat(n)
  wait
end

def philosopher(n)
  while true
    think n
    $forks[n].lock
    if not $forks[(n+1)%N].try_lock
      $forks[n].unlock		# avoid deadlock
      next
    end
    $state[n*2] = ?|;
    $state[(n+1)%N*2] = ?|;
    $state[n*2+1] = ?*;
    print $state, "\n"
    eat(n)
    $state[n*2] = ?-;
    $state[(n+1)%N*2] = ?-;
    $state[n*2+1] = ?o;
    print $state, "\n"
    $forks[n].unlock
    $forks[(n+1)%N].unlock
  end
end

for n in 0..N-1
  Thread.start(n){|i| philosopher(i)}
  sleep 0.1
end

sleep

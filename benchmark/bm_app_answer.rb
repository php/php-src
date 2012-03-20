def ack(m, n)
  if m == 0 then
    n + 1
  elsif n == 0 then
    ack(m - 1, 1)
  else
    ack(m - 1, ack(m, n - 1))
  end
end

def the_answer_to_life_the_universe_and_everything
  (ack(3,7).to_s.split(//).inject(0){|s,x| s+x.to_i}.to_s + "2" ).to_i
end

answer = the_answer_to_life_the_universe_and_everything

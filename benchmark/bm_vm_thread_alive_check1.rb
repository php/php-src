5_000.times{
  t = Thread.new{}
  while t.alive?
    Thread.pass
  end
}

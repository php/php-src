def ack(m, n)
    if m == 0 then
        n + 1
    elsif n == 0 then
        ack(m - 1, 1)
    else
        ack(m - 1, ack(m, n - 1))
    end
end

NUM = 9
ack(3, NUM)

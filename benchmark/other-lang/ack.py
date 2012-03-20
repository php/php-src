import sys
sys.setrecursionlimit(5000000)

def Ack(M, N):
    if (not M):
        return( N + 1 )
    if (not N):
        return( Ack(M-1, 1) )
    return( Ack(M-1, Ack(M, N-1)) )

def main():
    NUM = 9
    sys.setrecursionlimit(10000)
    Ack(3, NUM)

main()

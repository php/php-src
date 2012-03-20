#if __GNUC__ > 3

/* GCCE 4.3.2 generates these functions which are are missing from exports (they are simple aliases) */
extern int __aeabi_uidivmod(unsigned int a, unsigned int b);
extern int __aeabi_idivmod(int a, int b);
int __aeabi_idiv(int a, int b)
{
	return __aeabi_idivmod(a, b);
}

int __aeabi_uidiv(unsigned int a, unsigned int b)
{
	return __aeabi_uidivmod(a, b);
}

#endif



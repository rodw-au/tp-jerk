#ifndef RTAPI_BITOPS_H
#define RTAPI_BITOPS_H
#if (defined(__MODULE__) && !defined(SIM))
#include <asm/bitops.h>
#elif defined(__i386__)
/* From <asm/bitops.h>
 * Copyright 1992, Linus Torvalds.
 */

#define LOCK_PREFIX "lock ; "
#define ADDR (*(volatile long *) addr)

/**
 * set_bit - Atomically set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * This function is atomic and may not be reordered.  See __set_bit()
 * if you do not require the atomic guarantees.
 * Note that @nr may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 */
static __inline__ void set_bit(int nr, volatile void * addr)
{
	__asm__ __volatile__( 
		"btsl %1,%0"
		:"=m" (ADDR)
		:"Ir" (nr));
}

#if 0 /* Fool kernel-doc since it doesn't do macros yet */
/**
 * test_bit - Determine whether a bit is set
 * @nr: bit number to test
 * @addr: Address to start counting from
 */
static int test_bit(int nr, const volatile void * addr);
#endif

static __inline__ int constant_test_bit(int nr, const volatile void * addr)
{
	return ((1UL << (nr & 31)) & (((const volatile unsigned int *) addr)[nr >> 5])) != 0;
}

static __inline__ int variable_test_bit(int nr, volatile void * addr)
{
	int oldbit;

	__asm__ __volatile__(
		"btl %2,%1\n\tsbbl %0,%0"
		:"=r" (oldbit)
		:"m" (ADDR),"Ir" (nr));
	return oldbit;
}

#define test_bit(nr,addr) \
(__builtin_constant_p(nr) ? \
 constant_test_bit((nr),(addr)) : \
 variable_test_bit((nr),(addr)))

/**
 * clear_bit - Clears a bit in memory
 * @nr: Bit to clear
 * @addr: Address to start counting from
 *
 * clear_bit() is atomic and may not be reordered.  However, it does
 * not contain a memory barrier, so if it is used for locking purposes,
 * you should call smp_mb__before_clear_bit() and/or smp_mb__after_clear_bit()
 * in order to ensure changes are visible on other processors.
 */
static __inline__ void clear_bit(int nr, volatile void * addr)
{
	__asm__ __volatile__( 
		"btrl %1,%0"
		:"=m" (ADDR)
		:"Ir" (nr));
}

/**
 * test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.  
 * It also implies a memory barrier.
 */
static __inline__ int test_and_set_bit(int nr, volatile void * addr)
{
	int oldbit;

	__asm__ __volatile__( LOCK_PREFIX
		"btsl %2,%1\n\tsbbl %0,%0"
		:"=r" (oldbit),"=m" (ADDR)
		:"Ir" (nr) : "memory");
	return oldbit;
}


/**
 * test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.  
 * It also implies a memory barrier.
 */
static __inline__ int test_and_clear_bit(int nr, volatile void * addr)
{
	int oldbit;

	__asm__ __volatile__( 
		"btrl %2,%1\n\tsbbl %0,%0"
		:"=r" (oldbit),"=m" (ADDR)
		:"Ir" (nr) : "memory");
	return oldbit;
}
#elif defined(__x86_64__)
/*
 * Copyright 1992, Linus Torvalds.
 */


#define LOCK_PREFIX "lock ; "

#define ADDR (*(volatile long *) addr)

/**
 * set_bit - Atomically set a bit in memory
 * @nr: the bit to set
 * @addr: the address to start counting from
 *
 * This function is atomic and may not be reordered.  See __set_bit()
 * if you do not require the atomic guarantees.
 * Note that @nr may be almost arbitrarily large; this function is not
 * restricted to acting on a single-word quantity.
 */
static __inline__ void set_bit(int nr, volatile void * addr)
{
	__asm__ __volatile__( LOCK_PREFIX
		"btsl %1,%0"
		:"=m" (ADDR)
		:"dIr" (nr) : "memory");
}


/**
 * clear_bit - Clears a bit in memory
 * @nr: Bit to clear
 * @addr: Address to start counting from
 *
 * clear_bit() is atomic and may not be reordered.  However, it does
 * not contain a memory barrier, so if it is used for locking purposes,
 * you should call smp_mb__before_clear_bit() and/or smp_mb__after_clear_bit()
 * in order to ensure changes are visible on other processors.
 */
static __inline__ void clear_bit(int nr, volatile void * addr)
{
	__asm__ __volatile__( LOCK_PREFIX
		"btrl %1,%0"
		:"=m" (ADDR)
		:"dIr" (nr));
}


static __inline__ int constant_test_bit(int nr, const volatile void * addr)
{
	return ((1UL << (nr & 31)) & (((const volatile unsigned int *) addr)[nr >> 5])) != 0;
}

static __inline__ int variable_test_bit(int nr, volatile const void * addr)
{
	int oldbit;

	__asm__ __volatile__(
		"btl %2,%1\n\tsbbl %0,%0"
		:"=r" (oldbit)
		:"m" (ADDR),"dIr" (nr));
	return oldbit;
}

#define test_bit(nr,addr) \
(__builtin_constant_p(nr) ? \
 constant_test_bit((nr),(addr)) : \
 variable_test_bit((nr),(addr)))


/**
 * test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.  
 * It also implies a memory barrier.
 */
static __inline__ int test_and_set_bit(int nr, volatile void * addr)
{
	int oldbit;

	__asm__ __volatile__( LOCK_PREFIX
		"btsl %2,%1\n\tsbbl %0,%0"
		:"=r" (oldbit),"=m" (ADDR)
		:"dIr" (nr) : "memory");
	return oldbit;
}

/**
 * test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.  
 * It also implies a memory barrier.
 */
static __inline__ int test_and_clear_bit(int nr, volatile void * addr)
{
	int oldbit;

	__asm__ __volatile__( LOCK_PREFIX
		"btrl %2,%1\n\tsbbl %0,%0"
		:"=r" (oldbit),"=m" (ADDR)
		:"dIr" (nr) : "memory");
	return oldbit;
}
#undef ADDR
#elif defined(__powerpc__)

#define BITS_PER_LONG 32
#define BITOP_MASK(nr)          (1UL << ((nr) % BITS_PER_LONG))
#define BITOP_WORD(nr)          ((nr) / BITS_PER_LONG)

#ifdef CONFIG_SMP
#define ISYNC_ON_SMP    "\n\tisync\n"
#define LWSYNC_ON_SMP   __stringify(LWSYNC) "\n"
#else
#define ISYNC_ON_SMP
#define LWSYNC_ON_SMP
#endif

static __inline__ int test_and_set_bit(unsigned long nr,
                                       volatile unsigned long *addr)
{
        unsigned long old, t;
        unsigned long mask = BITOP_MASK(nr);
        unsigned long *p = ((unsigned long *)addr) + BITOP_WORD(nr);

        __asm__ __volatile__(
        LWSYNC_ON_SMP
"1:"    "lwarx  %0,0,%3              # test_and_set_bit\n"
        "or     %1,%0,%2 \n"
        "stwcx. %1,0,%3 \n"
        "bne-   1b"
        ISYNC_ON_SMP
        : "=&r" (old), "=&r" (t)
        : "r" (mask), "r" (p)
        : "cc", "memory");

        return (old & mask) != 0;
}

static __inline__ int test_and_clear_bit(unsigned long nr,
                                         volatile unsigned long *addr)
{
        unsigned long old, t;
        unsigned long mask = BITOP_MASK(nr);
        unsigned long *p = ((unsigned long *)addr) + BITOP_WORD(nr);

        __asm__ __volatile__(
        LWSYNC_ON_SMP
"1:"    "lwarx  %0,0,%3              # test_and_clear_bit\n"
        "andc   %1,%0,%2 \n"
        "stwcx. %1,0,%3 \n"
        "bne-   1b"
        ISYNC_ON_SMP
        : "=&r" (old), "=&r" (t)
        : "r" (mask), "r" (p)
        : "cc", "memory");

        return (old & mask) != 0;
}

#elif defined(__ARMEL__)

/*
 * CPU interrupt mask handling.
 */
#if ( __ARM_ARCH_6ZK__ | __ARM_ARCH_7A__ )

/** 
 * Yishin Li, ysli@araisrobo.com, 2011-12-30: 
 * I think we need at least ARMv6 which has hardware floating point
 * acceleration for trajectory planning calculation.
 **/

/*
 * Save the current interrupt enable state & disable IRQs
 */
static inline unsigned long arch_local_irq_save(void)
{
	unsigned long flags;

	asm volatile(
		"	mrs	%0, cpsr	@ arch_local_irq_save\n"
		"	cpsid	i"
		: "=r" (flags) : : "memory", "cc");
	return flags;
}

#else

#warning Is your (__ARM_ARCH__ < 6)?

/**
 * check your __ARM_ARCH_*__ with "gcc -dM -E - < /dev/null | grep ARM".
 * Then, append it to previous #if conditions.
 **/


/*
 * Save the current interrupt enable state & disable IRQs
 */
static inline unsigned long arch_local_irq_save(void)
{
	unsigned long flags, temp;

	asm volatile(
		"	mrs	%0, cpsr	@ arch_local_irq_save\n"
		"	orr	%1, %0, #128\n"
		"	msr	cpsr_c, %1"
		: "=r" (flags), "=r" (temp)
		:
		: "memory", "cc");
	return flags;
}

#endif

/*
 * restore saved IRQ & FIQ state
 */
static inline void arch_local_irq_restore(unsigned long flags)
{
	asm volatile(
		"	msr	cpsr_c, %0	@ local_irq_restore"
		:
		: "r" (flags)
		: "memory", "cc");
}


static __inline__ int test_and_set_bit(unsigned long bit,
                                       volatile unsigned long *p)
{
	unsigned long flags;
	unsigned int res;
	unsigned long mask = 1UL << (bit & 31);

	p += bit >> 5;

	flags = arch_local_irq_save();
	res = *p;
	*p = res | mask;
	arch_local_irq_restore(flags);

	return (res & mask) != 0;
}

static __inline__ int test_and_clear_bit(unsigned long bit,
                                         volatile unsigned long *p)
{
	unsigned long flags;
	unsigned int res;
	unsigned long mask = 1UL << (bit & 31);

	p += bit >> 5;

	flags = arch_local_irq_save();
	res = *p;
	*p = res & ~mask;
	arch_local_irq_restore(flags);

	return (res & mask) != 0;
}

#else
#error The header file <asm/bitops.h> is not usable and rtapi does not yet have support for your CPU
#endif
#endif

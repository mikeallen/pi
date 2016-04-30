/////////////////////////////////////////////////////////////////////////////
//
// Miscellaneous
//

#define EnableInts() sei();
#define DisableInts() cli();;

// Timer Divisors:
#define cClockDivideBy1		0b001
#define cClockDivideBy8		0b010
#define cClockDivideBy32	0b011
#define cClockDivideBy64	0b100
#define cClockDivideBy128	0b101
#define cClockDivideBy256	0b110
#define cClockDivideBy1024	0b111

#define cTimer0Divisor	8

// i.e. round to nearest mHz
#define cClocksPerUSec  4		// *** DEFINED IN ASM as WELL. Change both or none!
	
#define cClocksPerTick	(256 * cTimer0Divisor)

// note that cClocksPerTick/2 term is so that result is rounded to nearest int instead of 'floored'.
// and these are 'b2' ticks. (current value is 2 ticks/per msec)
#define	cTicksPerMSec	( ( (cClocksPerUSec * 1000) + (cClocksPerTick/2) ) / cClocksPerTick)

#define cTimer1DivisorsBits	cClockDivideBy1
#define cTimer1Divisor	1


///////////////////////////////////////////////////////////////////
// Misc Constants
///////////////////////////////////////////////////////////////////
#ifndef NULL
	#define NULL ((void *)0)
#endif

#define TRUE  (1)
#define FALSE (0) 

///////////////////////////////////////////////////////////////////
// Misc Macros
///////////////////////////////////////////////////////////////////

#ifdef DEBUG

void _AssertionFailure(char *exp);

#define BPAssert(_exp)  if (_exp) ; \
        else _AssertionFailure( #_exp )

#else

#define BPAssert(exp)

#endif 


// Access the lsb and msb, respectively, of a 16bit variable (only!)				
#define LowByte(_var) *((UInt8*)(&_var)+1)
#define HighByte(_var) *((UInt8*)(&_var))

// Access the lsb and msb, respectively, of a 32bit variable (only!)				
#define LowWord(_var) *((UInt16*)(&_var)+1)
#define HighWord(_var) *((UInt16*)(&_var))				

// From BBB code
#define Word1(l)    ((UInt2)(l))
#define Word2(l)    ((UInt2)(((UInt4)(l) >> 16) & 0xFFFF))
#define Byte1(w)    ((Byte)(w))
#define Byte2(w)    ((Byte)(((UInt2)(w) >> 8) & 0xFF))
#define Byte3(l)    ((Byte)(((UInt4)(l) >> 16) & 0xFF))
#define Byte4(l)    ((Byte)(((UInt4)(l) >> 24) & 0xFF))


#define SetWord1(l, val)    (l = ((l) & ~0xFFFF) | ((val) & 0xFFFF))
#define SetWord2(l, val)    (l = ((l) & 0xFFFF) | ((val) << 16))
#define SetByte1(w, val)    (w = ((w) & ~0xFF) | ((val) & 0xFF))
#define SetByte2(w, val)    (w = ((w) & 0xFF) | ( ((Byte)(val)) << 8) )
#define SetByte3(l, val)    (l = ((l) & 0xFF00FFFF) | ( ((Byte)(val)) << 16))
#define SetByte4(l, val)    (l = ((l) & 0x00FFFFFF) | ( ((Byte)(val)) << 24))



/*
      - VAR : Name of the character variable where the bit is located.
      - Place : Bit position in the variable (7 6 5 4 3 2 1 0)
      - Value : Can be 0 (reset bit) or not 0 (set bit)

	  The "BitMask" command allows to select some bits in a source
      variables and copy it in a destination var (return the value).

	  The "BitVal" command returns the value of a bit in a char 
      variable: the bit is reset if it returns 0 else the bit is set.
*/

#define BitSet(_var,_place)			( _var |= (1<<_place) )

#define BitClear(_var,_place)		( _var &= ((1<<_place)^255) )

#define BitFlip(_var,_place)		( _var ^= (1<<_place) )

#define BitAff(_var,_place,_value)	((_value) ? (_var |= (1<<_place)) : (_var &= ((1<<_place)^255)))

#define BitMask(_dest,_msk,_src)	( _dest = (_msk & _src) | ((~_msk) & _dest) )

#define BitVal(_var,_place)         (_var & (1<<_place))

#define BitTest(_var,_place)		(BitVal(_var,_place) != 0)

#define Bit(_place) 				(1<<_place)


#define Abs(a)	   (((a) < 0) ? -(a) : (a))


   
////////////////////////////////////////////
// Declarations for State Machines
//


#define cStateStop 255
   
#define MachineVariable(_machine) _machine ## Var
   
#define StateConst(_machine, _state) SN ## _machine ## _state

#define StateFunction(_machine, _state) _machine ## _state

// Initialize to stopped 
#define Machine(_machine) UInt1 MachineVariable(_machine) = cStateStop

   
// Used by a state machine to stop itself.
#define StateStop(_machine, _state) return(cStateStop)

// Used by one state machine (or other code) to stop another machine.
#define MachineStop(_machine) MachineVariable(_machine) = cStateStop


// Used by one state machine (or other code) to set the next state of another machine.
#define StateSet(_machine, _state) MachineVariable(_machine) = StateConst(_machine, _state)

   
#define State(_machine, _state) void StateFunction(_machine, _state) (void)
   
   
#define StateNext(_machine, _state)\
	if (1) {\
		StateSet(_machine, _state);\
		return;\
	} else


// for use inside state machine driver switch statement
#define StateCase(_machine, _state)\
	case StateConst(_machine, _state):\
		StateFunction(_machine, _state)();\
		break

//   
// ISR versions, since we can't call subroutines inside ISRs with Tiny compiler
//

#define MachineReturnLabel(_machine)	_Label ## _machine ## Ret

#define StateISR(_machine, _state) _Label ## _machine ## _state  :


#define StateNextISR(_machine, _state)\
	if (1) {\
		MachineVariable(_machine) = StateConst(_machine, _state);\
		goto MachineReturnLabel(_machine);\
	} else


#define StateCaseISR(_machine, _state)\
	case StateConst(_machine, _state):\
		goto _Label ## _machine ## _state ;\
		break




///////////////////////////////////////////////////////////////////
// Type definitions 
///////////////////////////////////////////////////////////////////

// Basic types (compiler-specific!)
typedef	unsigned char		UInt8;
typedef signed char		    Int8;
typedef unsigned short int	UInt16;
typedef signed short int	Int16;
typedef unsigned long		UInt32;
typedef signed long	        Int32;


typedef struct _Bits {		// Generic 8 bit port structure:
	unsigned int b0:1;
	unsigned int b1:1;
	unsigned int b2:1;
	unsigned int b3:1;
	unsigned int b4:1;
	unsigned int b5:1;
	unsigned int b6:1;
	unsigned int b7:1;
} Bits;

typedef union _BitsUInt8 {
	UInt8	all;
	Bits 	b;
} BitsUInt8;





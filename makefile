TARGET_LIB = libChipmunk.a
OBJS = chipmunk.o cpArbiter.o cpArray.o cpBB.o cpBody.o cpCollision.o cpHashSet.o cpPolyShape.o cpShape.o cpSpaceHash.o cpVect.o cpSpace.o  cpMouse.o\
             constraints/cpConstraint.o constraints/cpDampedRotarySpring.o constraints/cpDampedSpring.o constraints/cpGearJoint.o constraints/cpGrooveJoint.o constraints/cpPinJoint.o constraints/cpPivotJoint.o constraints/cpRatchetJoint.o constraints/cpRotaryLimitJoint.o constraints/cpSimpleMotor.o constraints/cpSlideJoint.o

PSPSDK = $(shell psp-config --pspsdk-path)
PSPPATH = $(shell psp-config --psp-prefix)

INCDIR = $(PSPPATH)/include
CFLAGS = -O2 -G0 -W -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

include $(PSPSDK)/lib/build.mak
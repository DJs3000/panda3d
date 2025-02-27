/**
 * PANDA 3D SOFTWARE
 * Copyright (c) Carnegie Mellon University.  All rights reserved.
 *
 * All use of this software is subject to the terms of the revised BSD
 * license.  You should have received a copy of this license along
 * with this source code in a file named "LICENSE."
 *
 * @file interrogate_interface.h
 * @author frang
 * @date 1999-11-09
 */

#ifndef INTERROGATE_INTERFACE_H
#define INTERROGATE_INTERFACE_H

#include "dtoolbase.h"

#ifdef __cplusplus
extern "C" {
#endif

// This file defines the interface to the interrogate database.  This database
// is generated by running interrogate on a package's source code; interrogate
// parses the C++ syntax, determines the public interface, generates C-style
// wrapper functions where necessary, and builds up a table of functions and
// classes and their relationships.

/*
 * Some of this data (in particular, the wrapper functions, and the table of
 * unique names for these functions) is linked in along with the codebase,
 * permanently a part of the library file, and is always available; the rest
 * of it is stored in external files (named *.in) and read in when needed.
 * For this reason, most of the interface functions defined here will force a
 * load of the complete interrogate database the first time any of them are
 * called.  The three exceptions are noted below; they are
 * interrogate_wrapper_has_pointer(), interrogate_wrapper_pointer(), and
 * interrogate_get_wrapper_by_unique_name().
 */


// The interface here is intentionally made to be as simple as possible, to
// maximize portability.  All that is required of a scripting language is a
// foreign function interface capable of calling C functions.


// In general, the interrogate database consists of a number of query
// functions that allow the caller to walk through the list of available
// types, functions, manifests, etc.  For each of these, a unique index number
// is returned; this index number may then be used to query details about the
// type, function, etc.  The index numbers are only guaranteed to remain
// unchanged during a particular session; from one session to another they may
// differ.

// All index numbers are ordinary integers.  Each has a unique typedef here
// for clarity of meaning, but they may be treated as ordinary integers by the
// caller.
typedef int ManifestIndex;
typedef int ElementIndex;
typedef int TypeIndex;
typedef int FunctionIndex;
typedef int FunctionWrapperIndex;
typedef int MakeSeqIndex;

// Atomic types are those that are built in to C.  This enumerated value is
// returned by interrogate_type_atomic_token() when a type is known to be one
// of the atomic types.
enum AtomicToken {
  AT_not_atomic = 0,
  AT_int = 1,
  AT_float = 2,
  AT_double = 3,
  AT_bool = 4,
  AT_char = 5,
  AT_void = 6,

  // There isn't an atomic string type in C, but there is one in almost all
  // other languages.  If -string is supplied to the interrogate command line,
  // functions may be reported as returning and accepting objects of type
  // atomic string.  For the C calling convention wrappers, atomic string
  // means (const char *); for other calling convention wrappers, atomic
  // string means whatever the native string representation is.
  AT_string = 7,

  AT_longlong = 8,

  // This is not a type that C has, but C++ and many scripting languages do;
  // it indicates a null value, or the absence of any value.
  AT_null = 9,
};

EXPCL_INTERROGATEDB void interrogate_add_search_directory(const char *dirname);
EXPCL_INTERROGATEDB void interrogate_add_search_path(const char *pathstring);
EXPCL_INTERROGATEDB bool interrogate_error_flag();

// Manifest Symbols

/*
 * These correspond to #define constants that appear in the C code.  (These
 * are only the manifest constants--those #define's that take no parameters.
 * Manifest functions, #define's that take one or more parameters, are not
 * exported.)  They cannot be set, of course, but they often have a meaningful
 * value that may be get.  The scripting language may choose to get the value
 * as a literal string via interrogate_manifest_definition(), or as a value of
 * a particular type (whatever type interrogate thinks it is), as returned by
 * the getter function given by interrogate_manifest_getter().
 */

EXPCL_INTERROGATEDB int interrogate_number_of_manifests();
EXPCL_INTERROGATEDB ManifestIndex interrogate_get_manifest(int n);
EXPCL_INTERROGATEDB ManifestIndex interrogate_get_manifest_by_name(const char *manifest_name);
EXPCL_INTERROGATEDB const char *interrogate_manifest_name(ManifestIndex manifest);
EXPCL_INTERROGATEDB const char *interrogate_manifest_definition(ManifestIndex manifest);
EXPCL_INTERROGATEDB bool interrogate_manifest_has_type(ManifestIndex manifest);
EXPCL_INTERROGATEDB TypeIndex interrogate_manifest_get_type(ManifestIndex manifest);
EXPCL_INTERROGATEDB bool interrogate_manifest_has_getter(ManifestIndex manifest);
EXPCL_INTERROGATEDB FunctionIndex interrogate_manifest_getter(ManifestIndex manifest);

// An exception is made for manifest constants that have an integer type
// value, since these are so common.  The scripting language can query these
// values directly, which saves having to generate a wrapper function for each
// stupid little manifest.  In this case, there will be no getter function
// available.
EXPCL_INTERROGATEDB bool interrogate_manifest_has_int_value(ManifestIndex manifest);
EXPCL_INTERROGATEDB int interrogate_manifest_get_int_value(ManifestIndex manifest);


// Data Elements

// These correspond to data members of a class, or global data elements.
// Interrogate automatically generates a getter function and, if possible, a
// setter function.

EXPCL_INTERROGATEDB const char *interrogate_element_name(ElementIndex element);
EXPCL_INTERROGATEDB const char *interrogate_element_scoped_name(ElementIndex element);
EXPCL_INTERROGATEDB bool interrogate_element_has_comment(ElementIndex element);
EXPCL_INTERROGATEDB const char *interrogate_element_comment(ElementIndex element);
EXPCL_INTERROGATEDB ElementIndex interrogate_get_element_by_name(const char *element_name);
EXPCL_INTERROGATEDB ElementIndex interrogate_get_element_by_scoped_name(const char *element_name);

// Be careful with this function.  The element's bare type is not likely to be
// directly useful to the scripting language.  This is a different answer than
// the return value of the getter.

// The element type might well be something concrete that the scripting
// language can't handle directly, e.g.  a Node, while the getter will return
// (and the setter accept) a pointer to a Node, which is what the scripting
// language actually works with.
EXPCL_INTERROGATEDB TypeIndex interrogate_element_type(ElementIndex element);

EXPCL_INTERROGATEDB bool interrogate_element_has_getter(ElementIndex element);
EXPCL_INTERROGATEDB FunctionIndex interrogate_element_getter(ElementIndex element);
EXPCL_INTERROGATEDB bool interrogate_element_has_setter(ElementIndex element);
EXPCL_INTERROGATEDB FunctionIndex interrogate_element_setter(ElementIndex element);

EXPCL_INTERROGATEDB bool interrogate_element_is_sequence(ElementIndex element);
EXPCL_INTERROGATEDB bool interrogate_element_is_mapping(ElementIndex element);

// Global Data

// This is the list of global data elements.

EXPCL_INTERROGATEDB int interrogate_number_of_globals();
EXPCL_INTERROGATEDB ElementIndex interrogate_get_global(int n);

// Functions

// There is a unique FunctionIndex associated with each of the functions that
// interrogate knows about.  This includes member functions, nonmember
// functions, synthesized getters and setters, and upcastdowncast functions.


// These are the global (nonmember) functions that appear outside of any class
// definition.
EXPCL_INTERROGATEDB int interrogate_number_of_global_functions();
EXPCL_INTERROGATEDB FunctionIndex interrogate_get_global_function(int n);

// This can be used to traverse through *all* the functions known to
// interrogate.  It's usually not what you want, since this includes global
// functions, class methods, and synthesized functions like upcasts and
// downcasts.  You probably want to use instead
// interrogate_number_of_global_functions(), above.
EXPCL_INTERROGATEDB int interrogate_number_of_functions();
EXPCL_INTERROGATEDB FunctionIndex interrogate_get_function(int n);

// This is the function's name.  It is not unique; it may be shared between
// multiple different functions that have the same name but different
// parameter types (this is C++'s function overloading). Two different classes
// might also have member functions that have the same name, or the same name
// as a global function (but also see the scoped_name, below).
EXPCL_INTERROGATEDB const char *interrogate_function_name(FunctionIndex function);

// The scoped name is the function name prefixed with the name of the class
// that includes the function, if the function is a class method.  If it is a
// global function, the scoped name is the same as the name returned above.
// In the absence of C++ function overloading, this name will be unique to
// each function.
EXPCL_INTERROGATEDB const char *interrogate_function_scoped_name(FunctionIndex function);

// This returns the C++ comment written for the function, either in the header
// file or in the .C file, or both.
EXPCL_INTERROGATEDB bool interrogate_function_has_comment(FunctionIndex function);
EXPCL_INTERROGATEDB const char *interrogate_function_comment(FunctionIndex function);

// This defines the function prototype as it appears in the C++ source, useful
// primarily for documentation purposes.
EXPCL_INTERROGATEDB const char *interrogate_function_prototype(FunctionIndex function);

// This can be used to determine the class that the function is a method for,
// if the function is a class method.
EXPCL_INTERROGATEDB bool interrogate_function_is_method(FunctionIndex function);
EXPCL_INTERROGATEDB TypeIndex interrogate_function_class(FunctionIndex function);
EXPCL_INTERROGATEDB bool interrogate_function_is_unary_op(FunctionIndex function);
EXPCL_INTERROGATEDB bool interrogate_function_is_operator_typecast(FunctionIndex function);
EXPCL_INTERROGATEDB bool interrogate_function_is_constructor(FunctionIndex function);
EXPCL_INTERROGATEDB bool interrogate_function_is_destructor(FunctionIndex function);

// This returns the module name reported for the function, if available.
EXPCL_INTERROGATEDB bool interrogate_function_has_module_name(FunctionIndex function);
EXPCL_INTERROGATEDB const char *interrogate_function_module_name(FunctionIndex function);

// This returns the library name reported for the function, if available.
EXPCL_INTERROGATEDB bool interrogate_function_has_library_name(FunctionIndex function);
EXPCL_INTERROGATEDB const char *interrogate_function_library_name(FunctionIndex function);

// This is true for virtual member functions.  It's not likely that this will
// be important to the scripting language.
EXPCL_INTERROGATEDB bool interrogate_function_is_virtual(FunctionIndex function);


// The actual callable function interface is defined via one or more wrappers
// for each function.  (There might be multiple wrappers for the same function
// to allow for default parameter values.)

// At present, interrogate can generate wrappers that use the C calling
// convention or the Python calling convention.  The set of wrappers that will
// actually be available depends on the parameters passed to the interrogate
// command line.
EXPCL_INTERROGATEDB int interrogate_function_number_of_c_wrappers(FunctionIndex function);
EXPCL_INTERROGATEDB FunctionWrapperIndex interrogate_function_c_wrapper(FunctionIndex function, int n);

EXPCL_INTERROGATEDB int interrogate_function_number_of_python_wrappers(FunctionIndex function);
EXPCL_INTERROGATEDB FunctionWrapperIndex interrogate_function_python_wrapper(FunctionIndex function, int n);

// Function wrappers

// These define the way to call a given function.  Depending on the parameters
// supplied to interrogate, a function wrapper may be able to supply either a
// void * pointer to the function, or the name of the function in the library,
// or both.


// This returns the actual name of the wrapper function, as opposed to the
// name of the function it wraps.  It's probably not terribly useful to the
// scripting language, unless the -fnames option was given to interrogate, in
// which case this name may be used to call the wrapper function (see
// is_callable_by_name, below).  It will usually be an ugly hashed name, not
// intended for human consumption.

// Don't confuse this with the unique_name, below.  The two are related, but
// not identical.
EXPCL_INTERROGATEDB const char *interrogate_wrapper_name(FunctionWrapperIndex wrapper);

// This returns true if -fnames was given to interrogate, making the wrapper
// function callable directly by its name.
EXPCL_INTERROGATEDB bool interrogate_wrapper_is_callable_by_name(FunctionWrapperIndex wrapper);

// This returns the C++ comment written for the function wrapper, usually from
// the .cpp file.  There may be a different comment for each overload of a
// given function.
EXPCL_INTERROGATEDB bool interrogate_wrapper_has_comment(FunctionWrapperIndex wrapper);
EXPCL_INTERROGATEDB const char *interrogate_wrapper_comment(FunctionWrapperIndex wrapper);

// Every function wrapper has zero or more parameters and may or may not have
// a return value.  Each parameter has a type and may or may not have a name.
// For member functions, the first parameter may be a 'this' parameter, which
// should receive a pointer to the class object.  (If a member function does
// not have a 'this' parameter as its first parameter, it is a static member
// function, also called a class method.)

EXPCL_INTERROGATEDB bool interrogate_wrapper_has_return_value(FunctionWrapperIndex wrapper);
EXPCL_INTERROGATEDB TypeIndex interrogate_wrapper_return_type(FunctionWrapperIndex wrapper);

/*
 * Sometimes interrogate must synthesize a wrapper that allocates its return
 * value from the free store.  Other times (especially if -refcount is
 * supplied to interrogate), interrogate will automatically increment the
 * count of a reference-counted object that it returns.  In cases like these,
 * interrogate_wrapper_caller_manages_return_value() will return true, and it
 * is the responsibility of the scripting language to eventually call the
 * destructor supplied by interrogate_wrapper_return_value_destructor() on
 * this value when it is no longer needed (which will generally be the same
 * destructor as that for the class).  Otherwise, this function will return
 * false, and the scripting language should *not* call any destructor on this
 * value.
 */
EXPCL_INTERROGATEDB bool interrogate_wrapper_caller_manages_return_value(FunctionWrapperIndex wrapper);
EXPCL_INTERROGATEDB FunctionIndex interrogate_wrapper_return_value_destructor(FunctionWrapperIndex wrapper);

// These define the parameters of the function.
EXPCL_INTERROGATEDB int interrogate_wrapper_number_of_parameters(FunctionWrapperIndex wrapper);
EXPCL_INTERROGATEDB TypeIndex interrogate_wrapper_parameter_type(FunctionWrapperIndex wrapper, int n);
EXPCL_INTERROGATEDB bool interrogate_wrapper_parameter_has_name(FunctionWrapperIndex wrapper, int n);
EXPCL_INTERROGATEDB const char *interrogate_wrapper_parameter_name(FunctionWrapperIndex wrapper, int n);
EXPCL_INTERROGATEDB bool interrogate_wrapper_parameter_is_this(FunctionWrapperIndex wrapper, int n);
EXPCL_INTERROGATEDB bool interrogate_wrapper_parameter_is_optional(FunctionWrapperIndex wrapper, int n);

// This returns a pointer to a function that may be called to invoke the
// function, if the -fptrs option to return function pointers was specified to
// interrogate.  Be sure to push the required parameters on the stack,
// according to the calling convention, before calling the function.

// These two functions may be called without forcing a load of the complete
// interrogate database.
EXPCL_INTERROGATEDB bool interrogate_wrapper_has_pointer(FunctionWrapperIndex wrapper);
EXPCL_INTERROGATEDB void *interrogate_wrapper_pointer(FunctionWrapperIndex wrapper);

// This function will return a name that is guaranteed to be unique to this
// particular function wrapper, and that will (usually) be consistent across
// multiple runtime sessions.  (It will only change between sessions if the
// database was regenerated in the interim with some new function that
// happened to introduce a hash conflict.)

// The unique name is an ugly hashed name, not safe for human consumption.
// Its sole purpose is to provide some consistent way to identify function
// wrappers between sessions.
EXPCL_INTERROGATEDB const char *interrogate_wrapper_unique_name(FunctionWrapperIndex wrapper);

// This function provides a reverse-lookup on the above unique name, returning
// the wrapper index corresponding to the given name.  It depends on data
// having been compiled directly into the library, and thus is only available
// if the option -unique-names was given to interrogate.

// This function may be called without forcing a load of the complete
// interrogate database.
EXPCL_INTERROGATEDB FunctionWrapperIndex interrogate_get_wrapper_by_unique_name(const char *unique_name);

// MakeSeqs

// These are special synthesized methods that iterate through a list.  They
// are generated in C++ code via the MAKE_SEQ macro.  The normal pattern is
// that a pair of actual C++ methods like get_num_things() and get_thing(n)
// are used to synthesize a new method called get_things().

EXPCL_INTERROGATEDB const char *interrogate_make_seq_seq_name(MakeSeqIndex make_seq);
EXPCL_INTERROGATEDB const char *interrogate_make_seq_scoped_name(MakeSeqIndex make_seq);
EXPCL_INTERROGATEDB bool interrogate_make_seq_has_comment(ElementIndex element);
EXPCL_INTERROGATEDB const char *interrogate_make_seq_comment(ElementIndex element);
// The name of the real method that returns the length, e.g.  "get_num_things"
EXPCL_INTERROGATEDB const char *interrogate_make_seq_num_name(MakeSeqIndex make_seq);
// The name of the real method that returns the nth element, e.g.  "get_thing"
EXPCL_INTERROGATEDB const char *interrogate_make_seq_element_name(MakeSeqIndex make_seq);
EXPCL_INTERROGATEDB FunctionIndex interrogate_make_seq_num_getter(MakeSeqIndex make_seq);
EXPCL_INTERROGATEDB FunctionIndex interrogate_make_seq_element_getter(MakeSeqIndex make_seq);

// Types

// These are all the types that interrogate knows about.  This includes atomic
// types like ints and floats, type wrappers like pointers and const pointers,
// enumerated types, and classes.

/*
 * Two lists of types are maintained: the list of global types, which includes
 * only those types intended to be wrapped in the API (for instance, all of
 * the classes).  The second list is the complete list of all types, which
 * probably does not need to be traversed--this includes *all* types known to
 * the interrogate database, including simple types and pointers and const
 * pointers to classes.  These types are necessary to fully define all of the
 * function parameters, but need not themselves be wrapped.
 */

EXPCL_INTERROGATEDB int interrogate_number_of_global_types();
EXPCL_INTERROGATEDB TypeIndex interrogate_get_global_type(int n);
EXPCL_INTERROGATEDB int interrogate_number_of_types();
EXPCL_INTERROGATEDB TypeIndex interrogate_get_type(int n);
EXPCL_INTERROGATEDB TypeIndex interrogate_get_type_by_name(const char *type_name);
EXPCL_INTERROGATEDB TypeIndex interrogate_get_type_by_scoped_name(const char *type_name);
EXPCL_INTERROGATEDB TypeIndex interrogate_get_type_by_true_name(const char *type_name);
EXPCL_INTERROGATEDB bool interrogate_type_is_global(TypeIndex type);
EXPCL_INTERROGATEDB const char *interrogate_type_name(TypeIndex type);
EXPCL_INTERROGATEDB const char *interrogate_type_scoped_name(TypeIndex type);
EXPCL_INTERROGATEDB const char *interrogate_type_true_name(TypeIndex type);

// A given type might be a nested type, meaning it is entirely defined within
// (and scoped within) some different C++ class.  In this case, the
// type_name() will return the local name of the type as seen within the
// class, while the scoped_name() will return the fully-qualified name of the
// type, and is_nested() and outer_class() can be used to determine the class
// it is nested within.
EXPCL_INTERROGATEDB bool interrogate_type_is_nested(TypeIndex type);
EXPCL_INTERROGATEDB TypeIndex interrogate_type_outer_class(TypeIndex type);

EXPCL_INTERROGATEDB bool interrogate_type_has_comment(TypeIndex type);
EXPCL_INTERROGATEDB const char *interrogate_type_comment(TypeIndex type);

// This returns the module name reported for the type, if available.
EXPCL_INTERROGATEDB bool interrogate_type_has_module_name(TypeIndex type);
EXPCL_INTERROGATEDB const char *interrogate_type_module_name(TypeIndex type);

// This returns the library name reported for the type, if available.
EXPCL_INTERROGATEDB bool interrogate_type_has_library_name(TypeIndex type);
EXPCL_INTERROGATEDB const char *interrogate_type_library_name(TypeIndex type);


// If interrogate_type_is_atomic() returns true, the type is one of the basic
// C types enumerated in AtomicToken, above.  The type may then be further
// modified by one or more of unsigned, signed, long, longlong, or short.
// However, it will not be a pointer.
EXPCL_INTERROGATEDB bool interrogate_type_is_atomic(TypeIndex type);
EXPCL_INTERROGATEDB AtomicToken interrogate_type_atomic_token(TypeIndex type);
EXPCL_INTERROGATEDB bool interrogate_type_is_unsigned(TypeIndex type);
EXPCL_INTERROGATEDB bool interrogate_type_is_signed(TypeIndex type);
EXPCL_INTERROGATEDB bool interrogate_type_is_long(TypeIndex type);
EXPCL_INTERROGATEDB bool interrogate_type_is_longlong(TypeIndex type);
EXPCL_INTERROGATEDB bool interrogate_type_is_short(TypeIndex type);

// If interrogate_type_is_wrapped() returns true, this is a composite type
// "wrapped" around some simpler type, for instance a pointer to a class.  The
// type will be either a pointer or a const wrapper--it cannot be a
// combination of these.  (When combinations are required, they use multiple
// wrappers.  A const char pointer, for example, is represented as a pointer
// wrapper around a const wrapper around an atomic char.)
EXPCL_INTERROGATEDB bool interrogate_type_is_wrapped(TypeIndex type);
EXPCL_INTERROGATEDB bool interrogate_type_is_pointer(TypeIndex type);
EXPCL_INTERROGATEDB bool interrogate_type_is_const(TypeIndex type);
EXPCL_INTERROGATEDB bool interrogate_type_is_typedef(TypeIndex type);
EXPCL_INTERROGATEDB TypeIndex interrogate_type_wrapped_type(TypeIndex type);

// If interrogate_type_is_array() returns true, this is an array type.
EXPCL_INTERROGATEDB bool interrogate_type_is_array(TypeIndex type);
EXPCL_INTERROGATEDB int interrogate_type_array_size(TypeIndex type);

// If interrogate_type_is_enum() returns true, this is an enumerated type,
// which means it may take any one of a number of named integer values.
EXPCL_INTERROGATEDB bool interrogate_type_is_enum(TypeIndex type);
EXPCL_INTERROGATEDB bool interrogate_type_is_scoped_enum(TypeIndex type);
EXPCL_INTERROGATEDB int interrogate_type_number_of_enum_values(TypeIndex type);
EXPCL_INTERROGATEDB const char *interrogate_type_enum_value_name(TypeIndex type, int n);
EXPCL_INTERROGATEDB const char *interrogate_type_enum_value_scoped_name(TypeIndex type, int n);
EXPCL_INTERROGATEDB const char *interrogate_type_enum_value_comment(TypeIndex type, int n);
EXPCL_INTERROGATEDB int interrogate_type_enum_value(TypeIndex type, int n);

// If none of the above is true, the type is some extension type.  It may be a
// struct, class, or union (and the distinction between these three is not
// likely to be important to the scripting language).  In any case, it may
// contain zero or more constructors, zero or one destructor, zero or more
// member functions, and zero or more data members; all of the remaining type
// functions may apply.
EXPCL_INTERROGATEDB bool interrogate_type_is_struct(TypeIndex type);
EXPCL_INTERROGATEDB bool interrogate_type_is_class(TypeIndex type);
EXPCL_INTERROGATEDB bool interrogate_type_is_union(TypeIndex type);

// If is_fully_defined() returns false, this classstruct was a forward
// reference, and we really don't know anything about it.  (In this case, it
// will appear to have no methods or members.)
EXPCL_INTERROGATEDB bool interrogate_type_is_fully_defined(TypeIndex type);

// If is_unpublished() returns false, the classstruct is unknown because it
// was not marked to be published (or, in promiscuous mode, it is a protected
// or private nested class).
EXPCL_INTERROGATEDB bool interrogate_type_is_unpublished(TypeIndex type);

/*
 * Otherwise, especially if the type is a struct or class, we may have a
 * number of member functions, including zero or more constructors and zero or
 * one destructor.  A constructor function may be called to allocate a new
 * instance of the type; its return value will be a pointer to the new
 * instance.  The destructor may be called to destroy the instance; however,
 * it usually should not be explicitly called by the user, since the proper
 * support of the interrogate_caller_manages_return_value() interface, above,
 * will ensure that the appropriate destructors are called when they should
 * be.
 */

/*
 * In certain circumstances, the destructor might be inherited from a parent
 * or ancestor class.  This happens when the destructor wrapper from the
 * ancestor class is an acceptable substitute for this destructor; this is
 * only possible in the case of a virtual C++ destructor.  In this case, the
 * destructor returned here will be the same function index as the one
 * returned by the ancestor class, and
 * interrogate_type_destructor_is_inherited() will return true for this class.
 */
EXPCL_INTERROGATEDB int interrogate_type_number_of_constructors(TypeIndex type);
EXPCL_INTERROGATEDB FunctionIndex interrogate_type_get_constructor(TypeIndex type, int n);
EXPCL_INTERROGATEDB bool interrogate_type_has_destructor(TypeIndex type);
EXPCL_INTERROGATEDB bool interrogate_type_destructor_is_inherited(TypeIndex type);
EXPCL_INTERROGATEDB FunctionIndex interrogate_type_get_destructor(TypeIndex type);

// This is the set of exposed data elements in the struct or class.
EXPCL_INTERROGATEDB int interrogate_type_number_of_elements(TypeIndex type);
EXPCL_INTERROGATEDB ElementIndex interrogate_type_get_element(TypeIndex type, int n);

// This is the set of exposed member functions in the struct or class.
EXPCL_INTERROGATEDB int interrogate_type_number_of_methods(TypeIndex type);
EXPCL_INTERROGATEDB FunctionIndex interrogate_type_get_method(TypeIndex type, int n);

// This is the set of MAKE_SEQ wrappers in the struct or class.
EXPCL_INTERROGATEDB int interrogate_type_number_of_make_seqs(TypeIndex type);
EXPCL_INTERROGATEDB MakeSeqIndex interrogate_type_get_make_seq(TypeIndex type, int n);

// A C++ class may also define a number of explicit cast operators, which
// define how to convert an object of this type to an object of some other
// type (the type can be inferred by the return type of the cast function).
// This is not related to upcast and downcast, defined below.
EXPCL_INTERROGATEDB int interrogate_type_number_of_casts(TypeIndex type);
EXPCL_INTERROGATEDB FunctionIndex interrogate_type_get_cast(TypeIndex type, int n);

// A C++ class may inherit from zero or more base classes.  This defines the
// list of base classes for this particular type.
EXPCL_INTERROGATEDB int interrogate_type_number_of_derivations(TypeIndex type);
EXPCL_INTERROGATEDB TypeIndex interrogate_type_get_derivation(TypeIndex type, int n);
EXPCL_INTERROGATEDB bool interrogate_type_is_final(TypeIndex type);

// For each base class, we might need to define an explicit upcast or downcast
// operation to convert the pointer to the derived class to an appropriate
// pointer to its base class (upcast) or vice-versa (downcast).  This is
// particularly true in the presence of multiple inheritance or virtual
// inheritance, in which case you cannot simply use the same pointer as either
// type.

// If interrogate_type_derivation_has_upcast() returns true for a particular
// typederivation combination, you must use the indicated upcast function to
// convert pointers of this type to pointers of the base type before calling
// any of the inherited methods from the base class.  If this returns false,
// you may simply use the same pointer as either a derived class pointer or a
// base class pointer without any extra step.
EXPCL_INTERROGATEDB bool interrogate_type_derivation_has_upcast(TypeIndex type, int n);
EXPCL_INTERROGATEDB FunctionIndex interrogate_type_get_upcast(TypeIndex type, int n);

/*
 * Although it is always possible to upcast a pointer to a base class, it is
 * not always possible to downcast from a base class to the derived class
 * (particularly in the presence of virtual inheritance).  If
 * interrogate_type_derivation_downcast_is_impossible() returns true, forget
 * it.  Otherwise, downcasting works the same way as upcasting.  (Of course,
 * it is the caller's responsibility to guarantee that the pointer actually
 * represents an object of the type being downcast to.)
 */
EXPCL_INTERROGATEDB bool interrogate_type_derivation_downcast_is_impossible(TypeIndex type, int n);
EXPCL_INTERROGATEDB bool interrogate_type_derivation_has_downcast(TypeIndex type, int n);
EXPCL_INTERROGATEDB FunctionIndex interrogate_type_get_downcast(TypeIndex type, int n);

// A C++ class may also define any number of nested types--classes or enums
// defined within the scope of this class.
EXPCL_INTERROGATEDB int interrogate_type_number_of_nested_types(TypeIndex type);
EXPCL_INTERROGATEDB TypeIndex interrogate_type_get_nested_type(TypeIndex type, int n);

#ifdef __cplusplus
}
#endif

#endif

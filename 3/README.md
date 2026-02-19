Your task is to implement class CRegister which implements a population register.

The class is essentially a database which records domiciles for each citizen. Moreover, we want to record the history of domiciles, as it was developing with time. Finally, we want to copy the database for backup reasons.

This problem is intended to practice shallow and deep copies of objects. The problem consists of mandatory, optional, and bonus tests. Correct implementation of the naive solution is required to pass the mandatory tests. A more advanced solution is required to pass the optional and bonus tests (and thus to earn more points). The improved solutions must conserve memory when creating object copies.

The aim is to practice and understand the difference between shallow and deep copy. Since deep copying is already handled in STL containers and in the C++ string objects, these are not available in this homework.

The required class CRegister has the following interface:

default constructor
    creates an empty instance - an empty register,
copy constructor
    creates a deep copy of the source instance. Implement it only if the automatically generated copy constructor does not work properly (seriously, you will have to implement it),
destructor
    frees the resources allocated by the instance
operator =
    to deep-copy data between instances,
add(id,name,surname,date,street,city)
    this method adds a new citizen into the register. The parameters are:

       - id is an unique identifier assigned to the person. You may assume it is always in the form "XXXXXX/YYYY",
       - name is a name, arbitrary length, any characters,
       - surname is a surname, arbitrary length, any characters,
       - date is a date, the start of the domicile, a string. You may assume it is a valid date in the format "YYYY-MM-DD",
       - street address, arbitrary length, any characters,
       - city address, arbitrary length, any characters.

    The method returns success(true) or error (false). An error is returned if the same person (same id) already exists in the register.
resettle(id,date,street,city)
    method adds a new domicile into the person's record. The parameters are the same as in the case of add. The method returns true to indicate success or false for a failure. There are two reasons for the failure: either there is no person with the given id in the register or there has already been a domicile change on that day for the given person (in other words, a person may change its domicile at most once a day). Caution: The changes of domicile may be processed in any order, i.e. the date does not have to form an increasing sequence.
print(stream,id)
    the method displays the domicile record for the given person identified by id into the given stream stream. The method returns true to indicate success or false to report error (there is no person with given id in the register). The output format is shown in sample runs. Please note the domiciles are listed such that the dates form an ascending sequence.

Submit a source file with your CRegister implementation. Moreover, add any further class (or classes) your implementation requires. The class must follow the public interface below. If there is a mismatch in the interface, the compilation will fail. You may extend the interface and add you auxiliary methods and member variables (both public and private, although private are preferred). The submitted file must include both declarations as well as implementation of the class (the methods may be implemented inline but do not have to). The submitted file shall not contain anything except the code needed to implement CRegister. In particular, if the file contains main, your tests, or any #include definitions, please, keep them in a conditional compile block. Use the template from the attached archive a basis for your implementation. If the preprocessor definitions are preserved, the file maybe submitted to Progtest.

Your implementation must not use STL (vector, list, ...). If you use any STL component, your program will fail to compile.

To pass the optional and bonus tests, design an efficient memory layout. The following advice may help:

    - the testing environment creates many copies where the original register and the copies are not further modified. Thus, it is a good idea to share data with the original at least until the first modification of either the original or the copy.
    - The changes of the register are "minor" in that the content is modified only slightly between the copies. Thus, it makes sense to share unmodified parts between the original and the copy. For instance, if you decide to create a class representing a person. Majority persons remains the same in both the original and the copy of the register.
    - The strings representing names (surnames, streets, cities) are highly redundant. There may be e.g., 10000 people in the register, however, there will be e.g., 500 unique surnames among them. Thus, it makes sense to share the strings among the register instance. Moreover, it may be a good idea to de-duplicate the names (surnames, ...), i.e., do a search for repeating strings.
    - The interface needs to find citizens based on their IDs, this search is included in all required CRegister methods. There may exist many citizens in the register, thus the search must be efficient. When a new instance of CRegister is created, it is likely to be populated with many new citizens. However, new citizens will not be added that frequently once the register is populated.
    - The speed test may be challenging even with the add operation. You might want to store the citizens into an array and you may want to keep some ordering in the array. Thus, there may be some overhead needed to shift the existing elements. This overhead itself may be so significant to fail the speed test. To address this problem either use C++ move semantics instead of copying, or re-design the array to hold pointers instead of structures/objects.

The required public interface of the class and example runs are included in the attached archive.


The task is to develop class CTable that simulates table similar to the HTML tables.

We assume a simplified table CTable. The table is formed by a fixed number of rows and a fixed number of columns. The number of rows/columns is given when an instance is initialized. A cell in the table may contain either an empty space (CEmpty), a text (CText), or an image (CImage). The table has interface to read/write its cells and the table may be displayed to a stream (the table adjusts its grid to fit all the cell contents without clipping).

### **CEmpty**

This class represents an empty cell in the table. The interface is just a default constructor to initialize the instance.

### **CText**

This class represents a cell with a formatted text. The text is displayed in the table cell, the newlines in the text must be respected. The text is adjusted to the left or right, based on a parameter. Vertical alignment is always "top". The interface of CText is:

- constructor CText(str,align)

    initializes a new instance of the class. The parameters are the text to display and the desired alignment (either ALIGN_LEFT or ALIGN_RIGHT).

- method setText ( str )

    the method sets new text to display. The alignment is not modified.

### **CImage**

This class represents a cell with an image. The image is represented in the form of "ASCII art" - i.e. a rectangle of characters. The image shall be horizontally and vertically centered in the table cell (if the extra space is not even, the image shall be placed one character to the left/up). The interface is:

- constructor CImage()

    initializes a new empty instance of the class.

- method addRow ( str )

    adds a row of character to the ASCII art image. You do not need to check the length, all rows will be equally long, thus the image will be of a rectangular shape.


Next, all classes implement operators == and != in the standard meaning (exact match/mismatch).

---

### **CTable**

This class represents a table. Individual cells may hold either empty space, text, or image. The class provides interface to access/modify cells and to display the formatted table:

- constructor (rows, cols)

    initializes the instance to the given size and sets all cells to empty.

- getCell (row,col)

    is used to access the contents of the given cell.

- setCell (row, col, newContent)

    sets new contents to the given cell.

- operators ==, !=

    are used to compare two tables (exact match/mismatch).

- operator <<

    is used to display the formatted table into an output stream.


---

Both STL and C++ string is available. However, the problem needs a good analysis before the coding. Use polymorphism when designing the classes. Indeed, the classes will not be accepted if they are not polymorphic.

Code sample is available in the attached archive.


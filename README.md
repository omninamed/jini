# jini
json-like initialization script parser

version overwiew:
- unbind from ATL, now all on STL
- settings structure added
- boolean synonym pairs added
- redesigned tree elements access from path, but for not all methods
- read buffer inside analyzer is now static for all steps
- analyzer bugs fixed when read bad-syntax files... probably not all errors ;)
- there is no error handling inside analyzer. all errors are skiped

engine features:
- linear structure - order of elements are equal to order in file
- simple and safe pair value convertation
- theoretically, any nesting depth of the objects
- read everything that can read ignore all errors
- existing objects/pairs are added/updated (sync read)
- when duplicat found and type of elements is not equal skip reading of entry element (and all subelements if it is object)

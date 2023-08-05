
:: Clean up any old files
rmdir /S /Q doxygen/

:: Re-generate the documentation
doxygen Doxyfile

:: Copy over Architecture diagrams.
xcopy out\lib\protocol_parser\Architecture\ProtocolParserLibraryArchitecture.png doxygen\html\ProtocolParserLibraryArchitecture.png /y
xcopy out\src\overallArchitecture.png doxygen\html\overallArchitecture.png /y

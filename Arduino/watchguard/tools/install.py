import json
import os
import os.path
import sys
import shutil


JSON_FILE_NAME = 'paths.json'

SOURCE_DIRECTORY_FIELD_NAME = 'source_library_directory_path'
DESTINATION_DIRECTORY_FIELD_NAME = 'arduino_library_directory_path'

FIELD_NAME_NOT_FOUND_TEMPLATE = 'Field "{}" is not defined.'
FIELD_TYPE_NOT_DIRECTORY_TEMPLATE = 'Value of field "{}" is not a directory.'
DIRECTORY_NO_ACCESS_TEMPLATE = 'Path "{}" lacks read and write permissions.'


def check_field_is_defined(field_name, json_dict):
    """Determines if the dictionary contains a specific entry. If the dictonary
    does not contain the entry, then the program exits with a message.
    """
    if field_name not in json_dict:
        sys.exit(FIELD_NAME_NOT_FOUND_TEMPLATE.format(field_name))


def check_field_is_directory(field_name, json_dict):
    """Determines if the dictionary contains an entry that is a valid path
    string to a directory on the machine. The directory must have read and write
    permissions to be valid. Otherwise, the program exits with a message.
    """
    field_value = json_dict[field_name]
    if type(field_value) != str or not os.path.isdir(field_value):
        sys.exit(FIELD_TYPE_NOT_DIRECTORY_TEMPLATE.format(field_value))
    if not os.access(field_value, os.R_OK | os.W_OK):
        sys.exit(DIRECTORY_NO_ACCESS_TEMPLATE.format(field_value))

    
def main():
    # Read json file for library source and library destination paths
    try:
        file = open(JSON_FILE_NAME, 'r')
        data = json.load(file)
        file.close()
    except FileNotFoundError:
        sys.exit('File "{}" could not be found.'.format(JSON_FILE_NAME))
    except json.JSONDecodeError:
        sys.exit('File "{}" is not valid json.'.format(JSON_FILE_NAME))

    # Check if fields values are valid
    check_field_is_defined(SOURCE_DIRECTORY_FIELD_NAME, data)
    check_field_is_directory(DESTINATION_DIRECTORY_FIELD_NAME, data)

    check_field_is_defined(SOURCE_DIRECTORY_FIELD_NAME, data)
    check_field_is_directory(DESTINATION_DIRECTORY_FIELD_NAME, data)
    
    # Copy library to destination
    source_directory = data[SOURCE_DIRECTORY_FIELD_NAME]
    destination_directory = data[DESTINATION_DIRECTORY_FIELD_NAME]

    for library in os.listdir(source_directory):
        library_path = os.path.join(source_directory, library)
        destination_path = os.path.join(destination_directory, library)
        shutil.copytree(library_path, destination_path)


if __name__ == '__main__':
    main()
# Purpose
Goals of this project is to scan the entire workbook below and extract externally managed notes (using either Inkdrop or Notion) to better organise note taking & bug resolutions.

Please keep in mind that this project is solely personal, i'll reject any feature request that don't match my personal need. Feel free to fork if you have any idea that better fit in your workflow.

Also as this project aims to be very lightweight, I've choosen C as the language here to also train myself using the language. Any pull requests that propose optimisation / code enhancement is welcome.

> [!IMPORTANT]
> Any coming that i suspect coming from an AI will be rejected, again, this is a PERSONAL project for PERSONAL usage and LEARNING purpose


# Integration
## Inkdrop
Main purpose of this project is to track Inkdrop tasks, this requires to have Inkdrop http server running locally with credentials set as environment variables

## Notion 
Notion will also be available to store the tasks and eventually track it better.

## Bitwarden
To avoid storing passwords & api key in plain environment variables. I'll try to include bitwarden configuration in this to allow user to use the bitwarden cli to store their credentials (especially Notion app key)

# Options
``--completed`` -> Also displays completed note, by default notes are hidden  
``--dir`` -> Specify a directory to scan, default scan the entire project from root excluding all files in the .env  
``--full`` -> Give the whole description for each tasks  

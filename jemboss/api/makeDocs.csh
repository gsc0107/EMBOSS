#!/bin/csh
/bin/rm Package-org *html stylesheet.css org/emboss/jemboss/* org/emboss/jemboss/gui/* org/emboss/jemboss/soap/* org/emboss/jemboss/programs/* org/emboss/jemboss/parser/* org/emboss/jemboss/gui/form/* org/emboss/jemboss/gui/sequenceChooser/* org/emboss/jemboss/parser/acd/* org/emboss/jemboss/gui/filetree/* org/emboss/jemboss/gui/startup/* 

/packages/java/jdk1.3/bin/javadoc -author -version -classpath :../lib/soap.jar:../lib/jalview.jar:../lib/activation.jar:../lib/mail.jar:../lib/jakarta-regexp-1.2.jar:../lib/xerces.jar:../org/emboss/Jemboss/:../:../embreo/embreo-full.jar: -package  org/emboss/jemboss/Jemboss org/emboss/jemboss/JembossLogo org/emboss/jemboss/JembossParams org/emboss/jemboss/gui/AdvancedOptions org/emboss/jemboss/gui/BuildProgramMenu org/emboss/jemboss/gui/LaunchJalView org/emboss/jemboss/gui/MemoryComboBox org/emboss/jemboss/gui/ResultsMenuBar org/emboss/jemboss/gui/SequenceData org/emboss/jemboss/gui/SequenceList org/emboss/jemboss/gui/SetUpMenuBar org/emboss/jemboss/gui/ShowResultSet org/emboss/jemboss/gui/ShowSavedResults org/emboss/jemboss/gui/SwingWorker org/emboss/jemboss/gui/filetree/DragTree org/emboss/jemboss/gui/filetree/FileEditorDisplay org/emboss/jemboss/gui/filetree/FileNode org/emboss/jemboss/gui/filetree/FileSave org/emboss/jemboss/gui/filetree/FileSaving org/emboss/jemboss/gui/filetree/RemoteDragTree org/emboss/jemboss/gui/filetree/RemoteFileNode org/emboss/jemboss/gui/filetree/RemoteFileTreePanel org/emboss/jemboss/gui/form/BuildJembossForm org/emboss/jemboss/gui/form/ListFilePanel org/emboss/jemboss/gui/form/MultiLineToolTipUI org/emboss/jemboss/gui/form/PlafMacros org/emboss/jemboss/gui/form/SectionPanel org/emboss/jemboss/gui/form/Separator org/emboss/jemboss/gui/form/SetInFileCard org/emboss/jemboss/gui/form/SetOutFileCard org/emboss/jemboss/gui/form/TextFieldFloat org/emboss/jemboss/gui/form/TextFieldInt org/emboss/jemboss/gui/form/TextFieldSink org/emboss/jemboss/gui/form/myComboPopup org/emboss/jemboss/gui/sequenceChooser/CutNPasteTextArea org/emboss/jemboss/gui/sequenceChooser/FileChooser org/emboss/jemboss/gui/sequenceChooser/InputSequenceAttributes org/emboss/jemboss/gui/sequenceChooser/OutputSequenceAttributes org/emboss/jemboss/gui/sequenceChooser/SequenceFilter org/emboss/jemboss/gui/sequenceChooser/TextAreaSink org/emboss/jemboss/gui/startup/Database org/emboss/jemboss/gui/startup/ProgList org/emboss/jemboss/parser/AcdFunResolve org/emboss/jemboss/parser/AcdVarResolve org/emboss/jemboss/parser/AcdVariableResolve org/emboss/jemboss/parser/Ajax org/emboss/jemboss/parser/AjaxUtil org/emboss/jemboss/parser/Dependent org/emboss/jemboss/parser/ParseAcd org/emboss/jemboss/parser/acd/Application org/emboss/jemboss/parser/acd/ApplicationFields org/emboss/jemboss/parser/acd/ApplicationParam org/emboss/jemboss/programs/BatchUpdateTimer org/emboss/jemboss/programs/JembossProcess org/emboss/jemboss/programs/ListFile org/emboss/jemboss/programs/ResultList org/emboss/jemboss/programs/ResultsUpdateTimer org/emboss/jemboss/programs/RunEmbossApplication org/emboss/jemboss/server/JembossFileServer org/emboss/jemboss/server/JembossServer org/emboss/jemboss/server/JembossThread org/emboss/jemboss/soap/AuthPopup org/emboss/jemboss/soap/CallAjax org/emboss/jemboss/soap/GetWossname org/emboss/jemboss/soap/JembossRun org/emboss/jemboss/soap/JembossSoapException org/emboss/jemboss/soap/PendingResults org/emboss/jemboss/soap/PrivateRequest org/emboss/jemboss/soap/ServerSetup



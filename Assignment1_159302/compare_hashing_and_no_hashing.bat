@echo off
@echo ====================================================================================================================================================
@echo                                            Hashing performance test  
@echo                                                  2018 version
@echo ====================================================================================================================================================
@echo EXPERIMENT (1)
@echo ====================================================================================================================================================
@echo INIT_STATE_#1:  042158367     GOAL STATE: 123804765
@echo ====================================================================================================================================================
@echo  ALGORITHM		         PATH_LENGTH	STATE_EXPANSIONS    MAX_QLENGTH	 RUNNING_TIME  DELETIONS_MIDDLE_HEAP ATTEMPTED_REEXPANSIONS
@echo ====================================================================================================================================================
set INIT_STATE="042158367"
set GOAL_STATE="123804765"

CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "Breadth_First_Search_VList"      %INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "Breadth_First_Search_VList"      %INIT_STATE% %GOAL_STATE%

CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "PDS_NonStrict_VList" 		%INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "PDS_NonStrict_VList" 		%INIT_STATE% %GOAL_STATE%
  
CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "aStar_ExpList_MisplacedTiles"      %INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "aStar_ExpList_MisplacedTiles"      %INIT_STATE% %GOAL_STATE%

CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "aStar_ExpList_Manhattan" %INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "aStar_ExpList_Manhattan" %INIT_STATE% %GOAL_STATE%

@echo ====================================================================================================================================================
@echo EXPERIMENT (2)
@echo ====================================================================================================================================================
@echo INIT_STATE_#2: 364058271     GOAL STATE: 123804765
@echo ====================================================================================================================================================
@echo  ALGORITHM		         PATH_LENGTH	STATE_EXPANSIONS    MAX_QLENGTH	 RUNNING_TIME  DELETIONS_MIDDLE_HEAP ATTEMPTED_REEXPANSIONS
@echo ====================================================================================================================================================
set INIT_STATE="364058271"
set GOAL_STATE="123804765"


CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "Breadth_First_Search_VList"      %INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "Breadth_First_Search_VList"      %INIT_STATE% %GOAL_STATE%

CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "PDS_NonStrict_VList" 		%INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "PDS_NonStrict_VList" 		%INIT_STATE% %GOAL_STATE%
  
CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "aStar_ExpList_MisplacedTiles"      %INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "aStar_ExpList_MisplacedTiles"      %INIT_STATE% %GOAL_STATE%

CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "aStar_ExpList_Manhattan" %INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "aStar_ExpList_Manhattan" %INIT_STATE% %GOAL_STATE%





@echo ====================================================================================================================================================
@echo EXPERIMENT (3)
@echo ====================================================================================================================================================
@echo INIT_STATE_#3: 281463075     GOAL STATE: 123804765
@echo ====================================================================================================================================================
@echo  ALGORITHM		         PATH_LENGTH	STATE_EXPANSIONS    MAX_QLENGTH	 RUNNING_TIME  DELETIONS_MIDDLE_HEAP ATTEMPTED_REEXPANSIONS
@echo ====================================================================================================================================================
set INIT_STATE="281463075"
set GOAL_STATE="123804765"

CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "Breadth_First_Search_VList"      %INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "Breadth_First_Search_VList"      %INIT_STATE% %GOAL_STATE%

CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "PDS_NonStrict_VList" 		%INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "PDS_NonStrict_VList" 		%INIT_STATE% %GOAL_STATE%
  
CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "aStar_ExpList_MisplacedTiles"      %INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "aStar_ExpList_MisplacedTiles"      %INIT_STATE% %GOAL_STATE%

CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "aStar_ExpList_Manhattan" %INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "aStar_ExpList_Manhattan" %INIT_STATE% %GOAL_STATE%



@echo ====================================================================================================================================================
@echo EXPERIMENT (4)
@echo ====================================================================================================================================================
@echo INIT_STATE_#4: 567408321     GOAL STATE: 123804765
@echo ====================================================================================================================================================
@echo  ALGORITHM		         PATH_LENGTH	STATE_EXPANSIONS    MAX_QLENGTH	 RUNNING_TIME  DELETIONS_MIDDLE_HEAP ATTEMPTED_REEXPANSIONS
@echo ====================================================================================================================================================
set INIT_STATE="567408321"
set GOAL_STATE="123804765"

CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "Breadth_First_Search_VList"      %INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "Breadth_First_Search_VList"      %INIT_STATE% %GOAL_STATE%

CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "PDS_NonStrict_VList" 		%INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "PDS_NonStrict_VList" 		%INIT_STATE% %GOAL_STATE%
  
CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "aStar_ExpList_MisplacedTiles"      %INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "aStar_ExpList_MisplacedTiles"      %INIT_STATE% %GOAL_STATE%

CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "aStar_ExpList_Manhattan" %INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "aStar_ExpList_Manhattan" %INIT_STATE% %GOAL_STATE%



@echo ====================================================================================================================================================
@echo EXPERIMENT (5)
@echo ====================================================================================================================================================
@echo INIT_STATE_#5: 463508721     GOAL STATE: 123804765
@echo ====================================================================================================================================================
@echo  ALGORITHM		         PATH_LENGTH	STATE_EXPANSIONS    MAX_QLENGTH	 RUNNING_TIME  DELETIONS_MIDDLE_HEAP ATTEMPTED_REEXPANSIONS
@echo ====================================================================================================================================================
set INIT_STATE="463508721"
set GOAL_STATE="123804765"

CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "Breadth_First_Search_VList"      %INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "Breadth_First_Search_VList"      %INIT_STATE% %GOAL_STATE%

CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "PDS_NonStrict_VList" 		%INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "PDS_NonStrict_VList" 		%INIT_STATE% %GOAL_STATE%
  
CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "aStar_ExpList_MisplacedTiles"      %INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "aStar_ExpList_MisplacedTiles"      %INIT_STATE% %GOAL_STATE%

CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "aStar_ExpList_Manhattan" %INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "aStar_ExpList_Manhattan" %INIT_STATE% %GOAL_STATE%

@echo ====================================================================================================================================================
@echo EXPERIMENT (6)
@echo ====================================================================================================================================================
@echo INIT_STATE_#5: 471506238     GOAL STATE: 123804765
@echo ====================================================================================================================================================
@echo  ALGORITHM		         PATH_LENGTH	STATE_EXPANSIONS    MAX_QLENGTH	 RUNNING_TIME  DELETIONS_MIDDLE_HEAP ATTEMPTED_REEXPANSIONS
@echo ====================================================================================================================================================
set INIT_STATE="471506238"
set GOAL_STATE="123804765"

CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "Breadth_First_Search_VList"      %INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "Breadth_First_Search_VList"      %INIT_STATE% %GOAL_STATE%

CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "PDS_NonStrict_VList" 		%INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "PDS_NonStrict_VList" 		%INIT_STATE% %GOAL_STATE%
  
CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "aStar_ExpList_MisplacedTiles"      %INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "aStar_ExpList_MisplacedTiles"      %INIT_STATE% %GOAL_STATE%

CALL run_with_timeout.bat mainNoHashing.exe "batch_run" "aStar_ExpList_Manhattan" %INIT_STATE% %GOAL_STATE%
CALL run_with_timeout.bat main.exe "batch_run" "aStar_ExpList_Manhattan" %INIT_STATE% %GOAL_STATE%

@echo nothing follows.
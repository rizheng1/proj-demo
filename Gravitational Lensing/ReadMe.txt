Project progress log. This is here to 
record what have beeen tried and failed. (or succeeded) 

1. 
First Attempt, strategy is to copy LightmassImportanceVolume (LIV). Created LightmassGravityVolume based on LIV, and plugged it into
associated files LightingSystems.cpp, LightingSystems.h, Lightmass.cpp, Lightmass.h, LightmassScene.cpp, LightmassScene.h.
Copied functionality of LIV in corresponding files.
This approach was scrapped because Importer.ImportData() function in LightmassScene.cpp did not correctly retrieve objects. 
Autmenting ImportData would change engine code, which seemed too much out side the scope of the project, thus the approach was 
scrapped.
Files in Bad Files/Bad1

2.
Second Attempt, strategy is to modify LightmassImportanceVolume and collision behavior. In Collision.cpp, L303 is a boolean which keeps track of whether ray 
is in the importance volume. The adjustment for gravity dialiation is done immediately inside the do loop L312 on the ray object LightRay.
Files in Current 

Video Link: https://drive.google.com/open?id=1NAfl9RaYHFCj9CkIGlM4bzUFCyJsNKTv

3. 
Yay, the project is working! The previous iteration did not work because it just didn't get invoked enough. In this iteration, 
I went directly to the LightRay object's constructor and hard coded the gravitational shift effect in there. I still need to tweak the
constants a bit and make a presentation level for the slides.

Video link: https://drive.google.com/open?id=1wavJNhTQGSyAbaJ_sbbh7_wVV8ViLAOK
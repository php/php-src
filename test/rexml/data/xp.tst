/
/rss
//rss
/rss/channel
//link
//image/*
//link[2]
//link[last()]
rss/channel/link[last()]
rss/channel/item/link[last()]		      
rss/channel/item/link[1] 		          
rss/channel/item[@x='1']              
rss/channel/item[@x]                  
//item[@x]                            
//item[normalize-space(@name)='x']		
//*[count(title)=1] 		              
//*[name()='link']                    
//*[starts-with(name(),'li')] 	    	
//*[contains(name(),'y')] 		        
//*[string-length(name()) = 4] 		    
//copyright | //title 		            
/child::rss 		                      
/descendant::* 		                    
//language/parent::*		              
/rss/channel/ancestor::*		          
//item[position() mod 2 = 0 ]
//item/ancestor::*
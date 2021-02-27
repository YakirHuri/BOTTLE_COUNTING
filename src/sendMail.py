# importing the required library. 
import smtplib 
import sys


mailToSend = sys.argv[1]
finalMsg = sys.argv[2]

# creates SMTP session 
email = smtplib.SMTP('smtp.gmail.com', 587) 
  
# TLS for security 
email.starttls() 
  
# authentication
# compiler gives an error for wrong credential. 
email.login("yakirhuri21@gmail.com", "yakir26keva") 
  
# message to be sent 
message = """From: %s\nTo: %s\nSubject: %s\n\n%s""" % (mailToSend, ", ".join("Finalproject649@gmail.com"), finalMsg, " bala")


# sending the mail 
email.sendmail(mailToSend, lectureName, message) 
  
# terminating the session 
email.quit()
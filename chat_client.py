# TCP chat client
import socket, select, string, sys, os
 
def prompt() :
    sys.stdout.write('<You> ')
    sys.stdout.flush()

def QoS_measure():
    intentos = 0
    exitos = 0
    umbral = 0.5
    while (intentos<100):
        s.send('Metalico Hola')
        respuesta = s.recv(4096)
        while (respuesta=='Try!'):
            intentos += 1
            respuesta = s.recv(4096)
            if (respuesta=='Success!'):
                exitos += 1
            if (respuesta=='Stop!'):
                os.system("pkill -f top_block.py") # mato el top_block para reiniciarlo
                os.system("xfce4-terminal -x sh -c 'python top_block.py; bash'")
    tasa_exito = 100*float(exitos)/float(intentos)
    print str(int(tasa_exito)) + '%'


#main function
if __name__ == "__main__":
     
    if(len(sys.argv) < 3) :
        print 'Usage : python telnet.py hostname port'
        sys.exit()
     
    host = sys.argv[1]
    port = int(sys.argv[2])
     
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(200)
     
    # connect to remote host
    try :
        s.connect((host, port))
    except :
        print 'Unable to connect'
        sys.exit()
     
    print 'Connected to remote host. Start sending messages'
    prompt()
     
    while 1:
        socket_list = [sys.stdin, s]
         
        # Get the list sockets which are readable
        read_sockets, write_sockets, error_sockets = select.select(socket_list , [], [])
         
        for sock in read_sockets:
            #incoming message from remote server
            if sock == s:
                data = sock.recv(4096)
                if not data :
                    print '\nDisconnected from chat server'
                    sys.exit()
                else :
                    #print data
                    sys.stdout.write(data)
                    prompt()
                    #print '\nEmpezando medida de la QoS'
                    QoS_measure()

            #user entered a message
            else :
                msg = sys.stdin.readline()
                s.send(msg)
                prompt()

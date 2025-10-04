import time #Utilizado para determinar os atraso entre as medições
import os # biblioteca responsável pela manipulação do arquivo de log
import datetime #obter a data e hora
from periphery import I2C #comunicação I2C 

# Endereço I2C do sensor BH1750
ADDR = 0x23

# Inicializa a comunicação I2C
try:
    i2c = I2C("/dev/i2c-2")
except Exception as e:
    print(f"Erro ao inicializar o I2C: {e}")
    i2c = None

def setup_sd_card():

    # verifica o caminho do cartão microSD 
    sd_path = "/media/caninos/BOOT"
    if not os.path.ismount(sd_path):
        print("Erro: Cartão SD não montado. Verifique a montagem.")
        return None
    return sd_path

# data e hora atual no formato YYYY-MM-DD HH:MM:SS
def get_timestamp():
    return datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S') 

def main():
    if not i2c:
        print("I2C não está disponível. Encerrando o programa.")
        return

    #configura o caminho do cartão SD
    sd_card_path = setup_sd_card()
    if not sd_card_path:
        return

    #ome do arquivo de log
    log_file = os.path.join(sd_card_path, "medicao_luminus.txt")

    print(f"Iniciando a gravação no cartão SD. Os dados serão salvos em: {log_file}")

    #cria e escreve o cabeçalho no arquivo (se ele não existir)
    if not os.path.exists(log_file):
        with open(log_file, "a") as f:
            f.write("Timestamp, Luminosidade (lux:.2f)\n")
    
    #loop de medição e gravação
    try:
        i2c.transfer(ADDR, [I2C.Message([0x10])])
        time.sleep(0.2)
        
        while True:
            # cria uma mensagem para ler 2 bytes do sensor
            msg = I2C.Message([0x00, 0x00], read=True)
            i2c.transfer(ADDR, [msg])
            data = msg.data
            
            # converte os bytes lidos em lux
            lux = (data[0] << 8 | data[1]) / 1.2

            timestamp = get_timestamp()
            data_lux = f"{timestamp}\nLuminosidade: {lux:.2f}\n"

            # escreve a linha no arquivo no modo de 'append' utilizando "a"
            with open(log_file, "a") as f:
                f.write(data_lux)

            print(f"{data_lux.strip()}")
            
            # tempo de atraso entre as medições: 1 em 1 segundo 
            time.sleep(1)

    except KeyboardInterrupt:
        print("Gravação interrompida pelo usuário.")
    except Exception as e:
        print(f"Ocorreu um erro: {e}")

if __name__ == "__main__":
    main()

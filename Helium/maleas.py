import tkinter as tk
import random

def create_oxidation():
    # Cria uma pequena janela flutuante
    root = tk.Tk()
    
    # Remove as bordas e a barra de título (fica só o texto)
    root.overrideredirect(True)
    
    # Define o fundo como transparente (ou preto para destacar)
    root.config(bg='black')
    
    # Atributos do texto
    label = tk.Label(
        root, 
        text="Gas", 
        font=("Arial Black", random.randint(20, 50)), 
        fg="#8800FF", # Laranja
        bg="black"
    )
    label.pack()

    # Define uma posição aleatória na tela
    screen_width = root.winfo_screenwidth()
    screen_height = root.winfo_screenheight()
    x = random.randint(0, screen_width - 200)
    y = random.randint(0, screen_height - 100)
    
    root.geometry(f"+{x}+{y}")

    # Garante que a janela fique em cima de tudo
    root.lift()
    root.attributes("-topmost", True)

    # Faz a janela fechar sozinha depois de 1.5 segundos (efeito fade)
    root.after(1500, root.destroy)
    
    # Agenda a próxima aparição
    root.after(300, create_oxidation)
    
    root.mainloop()

if __name__ == "__main__":
    print("Pressione Ctrl+C no terminal para parar o efeito.")
    create_oxidation()
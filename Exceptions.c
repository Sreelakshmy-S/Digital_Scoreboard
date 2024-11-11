// Bus Fault Handler
void BusFault_Handler(void) {
    // Log the fault or send a message
    USART1_SendString("Bus Fault detected!\r\n");

    // Implement a safe recovery or system reset
    while (1) {
        // Optionally blink an LED to indicate a fault
    }
}

//Memory Management Fault Handle
void MemManage_Handler(void) {
    // Log the fault or send a message
    USART1_SendString("Memory Management Fault detected!\r\n");

    // Implement a safe recovery or system reset
    while (1) {
        // Optionally blink an LED to indicate a fault
    }
}

//Hard Fault Handler
void HardFault_Handler(void) {
    // Log the fault or send a message
    USART1_SendString("Hard Fault detected!\r\n");

    // Implement a safe recovery or system reset
    while (1) {
        // Optionally blink an LED to indicate a fault
    }
}

//Default Exception Handler
void Default_Handler(void) {
    // Log the fault or send a message
    USART1_SendString("Unknown Exception detected!\r\n");

    // Implement a safe recovery or system reset
    while (1) {
        // Optionally blink an LED to indicate a fault
    }
}

//Default Exception Handler
void Default_Handler(void) {
    // Log the fault or send a message
    USART1_SendString("Unknown Exception detected!\r\n");

    // Implement a safe recovery or system reset
    while (1) {
        // Optionally blink an LED to indicate a fault
}
}

//Command Validation: Ensure that received UART commands are valid.
void Check_UART_Command(void) {
    if (USART1->SR & USART_SR_RXNE) {
        char received_char = USART1_ReceiveChar();
        if (received_char == '1') {
            Increment_Score1();
        } else if (received_char == '2') {
            Increment_Score2();
        } else {
            // Handle invalid command
            USART1_SendString("Invalid command received.\r\n");
        }
    }
}

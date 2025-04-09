import 'package:flutter/material.dart';

class ConfiguracionArduinoPage extends StatelessWidget {
  const ConfiguracionArduinoPage({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Configuración Arduino')),
      body: Center(
        child: Text('Esta es la pantalla de configuración de Arduino'),
      ),
    );
  }
}

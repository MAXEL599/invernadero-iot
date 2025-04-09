import 'package:flutter/material.dart';

class AjustesPage extends StatelessWidget {
  const AjustesPage({super.key});

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text('Ajustes')),
      body: Center(child: Text('Esta es la pantalla de ajustes')),
    );
  }
}
